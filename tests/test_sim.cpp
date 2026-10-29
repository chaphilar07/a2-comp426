#include "grid.h"
#include "medicine.h"
#include "sim.h"
#include <cmath>
#include <cstdio>

static int failed = 0;
#define CHECK(cond, name)                                                      \
  do {                                                                         \
    printf("%s  %s\n", (cond) ? "PASS" : "FAIL", name);                        \
    if (!(cond))                                                               \
      failed++;                                                                \
  } while (0)

/* put count cells of type around (x, y) in the cell grid */
static void surround(int x, int y, int count, uint8_t type) {
  int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1}, dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  for (int k = 0; k < count; k++)
    cur[y + dy[k]][x + dx[k]] = type;
}

/* put count medicine cells around (x, y), as a tiny injection whose
 * particles do not move (zero direction), so the rules see them in place */
static void surround_med(int x, int y, int count) {
  int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1}, dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  sim::injections.emplace_back();
  injection &inj = sim::injections.back();
  for (int k = 0; k < count; k++) {
    particle q = {(float)(x + dx[k]), (float)(y + dy[k]), 0, 0, true};
    inj.p.push_back(q);
  }
}

static void build(void) {
  grid_init(0.26, 42);
  sim::add_injection(256, 192, 3000);
  sim::add_injection(768, 192, 2000);
  sim::add_injection(256, 576, 1500);
  sim::add_injection(1020, 760, 1000); /* partly off the grid */
}

int main(void) {
  /* rule 1 */
  grid_reset();
  sim::init(1);
  surround(100, 100, 6, CANCER);
  surround(200, 200, 5, CANCER);
  sim::step();
  CHECK(cur[100][100] == CANCER, "rule 1: 6 cancer neighbours -> cancer");
  CHECK(cur[200][200] == HEALTHY,
        "rule 1: 5 cancer neighbours -> stays healthy");

  /* rule 2 */
  grid_reset();
  sim::init(1);
  cur[100][100] = CANCER;
  surround_med(100, 100, 6);
  cur[200][200] = CANCER;
  surround_med(200, 200, 5);
  sim::step();
  CHECK(cur[100][100] == HEALTHY, "rule 2: 6 medicine neighbours -> cured");
  CHECK(cur[200][200] == CANCER,
        "rule 2: 5 medicine neighbours -> stays cancer");
  sim::step();
  CHECK(sim::injections[0].alive() == 0,
        "absorb: the 6 medicine cells are used up next step");
  CHECK(sim::injections[1].alive() == 5,
        "absorb: medicine around an uncured cell stays");

  /* rule 3: with no cancer, every medicine cell moves 1 cell per step */
  grid_reset();
  sim::init(1);
  sim::add_injection(500, 400, 200);
  injection &one = sim::injections[0];
  float x0 = one.p[7].x, y0 = one.p[7].y;
  int before = one.alive();
  for (int s = 0; s < 10; s++)
    sim::step();
  CHECK(one.alive() == before, "move: no medicine lost on an empty grid");
  CHECK(std::fabs(std::hypot(one.p[7].x - x0, one.p[7].y - y0) - 10.0f) < 0.01f,
        "move: 10 steps -> 10 cells outward");
  CHECK(grid_count().medicine == before,
        "move: medicine layer shows every live cell");

  /* edge */
  for (int s = 0; s < 1200; s++)
    sim::step();
  CHECK(one.alive() == 0, "edge: all medicine gone after leaving the grid");
  CHECK(grid_count().medicine == 0, "edge: no yellow left on the grid");

  /* threads: same result with 1 thread and with every thread */
  sim::init(1);
  build();
  for (int s = 0; s < 100; s++)
    sim::step();
  unsigned long long single = grid_checksum();
  sim::init(0);
  build();
  for (int s = 0; s < 100; s++)
    sim::step();
  CHECK(grid_checksum() == single,
        "threads: all threads == 1 thread, same checksum");
  printf("(%d threads)\n", sim::threads());

  printf("\n%s\n", failed ? "SOME TESTS FAILED" : "ALL TESTS PASSED");
  return failed;
}
