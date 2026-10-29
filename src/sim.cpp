#include "sim.h"
#include "grid.h"
#include "rules.h"
#include <tbb/blocked_range.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/parallel_for.h>
#include <tbb/partitioner.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/tick_count.h>

namespace sim {

std::vector<injection> injections;

static tbb::task_arena *arena = nullptr;
static int n_threads = 0;

/* One counter per worker thread: no sharing, no atomics. Combined only when
 * the control thread asks for the throughput, between steps. */
static tbb::enumerable_thread_specific<unsigned long long> counts;
static double busy_seconds = 0;
static unsigned long long n_steps = 0;

/* Remembers which thread ran which rows last step, so the same rows land in
 * the same core's cache again. */
static tbb::affinity_partitioner rows_ap;

enum { ROW_GRAIN = 8, PARTICLE_GRAIN = 1024 };

void init(int threads) {
  n_threads = threads > 0 ? threads : tbb::this_task_arena::max_concurrency();
  delete arena;
  arena = new tbb::task_arena(n_threads);
  counts.clear();
  busy_seconds = 0;
  n_steps = 0;
  injections.clear();
}

void add_injection(int cx, int cy, int n) {
  injections.emplace_back();
  injections.back().init(cx, cy, n);
}

/* Phase A: one task per injection. Inside a task the particles are split
 * into chunks with parallel_for, so one large injection is not the critical
 * path while the other tasks are already finished. */
static void inject_all(tbb::task_group &tg, uint32_t prev) {
  for (injection &inj : injections) {
    tg.run([&inj, prev] {
      tbb::parallel_for(
          tbb::blocked_range<size_t>(0, inj.p.size(), PARTICLE_GRAIN),
          [&inj, prev](const tbb::blocked_range<size_t> &r) {
            inj.move(r.begin(), r.end(), prev);
            counts.local() += r.end() - r.begin();
          });
    });
  }
}

/* Phase B: the rules over bands of rows. Each chunk writes only its own rows
 * of next and rgb, so no two chunks ever write the same cell. */
static void rules_all() {
  tbb::parallel_for(
      tbb::blocked_range<int>(0, H, ROW_GRAIN),
      [](const tbb::blocked_range<int> &r) {
        rules_rows(r.begin(), r.end());
        counts.local() += (unsigned long long)(r.end() - r.begin()) * W;
      },
      rows_ap);
}

void step() {
  tbb::tick_count t0 = tbb::tick_count::now();
  arena->execute([] {
    uint32_t prev = stamp;
    stamp = prev + 1;
    tbb::task_group tg;
    inject_all(tg, prev);
#ifdef BROKEN_NO_JOIN
    rules_all(); /* wrong on purpose: rules run while medicine still moves */
    tg.wait();
#else
    tg.wait();   /* join 1: all medicine in place before any rule runs */
    rules_all(); /* join 2: all rules done before the control thread goes on */
#endif
  });
  grid_swap();
  busy_seconds += (tbb::tick_count::now() - t0).seconds();
  n_steps++;
}

int threads() { return n_threads; }

double throughput() {
  unsigned long long total = 0;
  for (unsigned long long c : counts)
    total += c;
  return busy_seconds > 0 ? total / busy_seconds : 0;
}

double step_ms() { return n_steps ? busy_seconds * 1000 / n_steps : 0; }

} // namespace sim
