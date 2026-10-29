#include "grid.h"
#include "render.h"
#include "sim.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static char text[160];
static int frame = 0;

static void step(void) {
  sim::step();
  if (++frame % 30 == 0)
    printf("%s\n", text);
}
static const char *status(void) {
  snprintf(text, sizeof text,
           "%d TBB threads   %zu injections   %.1f million computations/s   "
           "%.2f ms/step",
           sim::threads(), sim::injections.size(), sim::throughput() / 1e6,
           sim::step_ms());
  return text;
}

int main(int argc, char **argv) {
  int threads = 0, i = 1;
  if (i + 1 < argc && strcmp(argv[i], "-t") == 0) {
    threads = atoi(argv[i + 1]);
    i += 2;
  }
  grid_init(0.26, 42);
  sim::init(threads);

  for (; i < argc; i++) {
    int x, y, n;
    if (sscanf(argv[i], "%d,%d,%d", &x, &y, &n) == 3 && n > 0)
      sim::add_injection(x, y, n);
    else
      fprintf(stderr, "ignored argument: %s\n", argv[i]);
  }
  if (sim::injections.empty()) {
    sim::add_injection(800, 150, 2500);
    sim::add_injection(300, 250, 2000);
    sim::add_injection(700, 500, 1500);
    sim::add_injection(512, 384, 1000);
  }

  grid_counts c = grid_count();
  printf("cancer cells: %.1f%%   injections: %zu   TBB threads: %d\n",
         100.0 * c.cancer / (c.cancer + c.healthy), sim::injections.size(),
         sim::threads());
  render_run(&argc, argv, step, status);
}
