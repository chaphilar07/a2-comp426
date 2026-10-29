
#include "grid.h"
#include "sim.h"
#include <cstdio>
#include <cstdlib>

#define STEPS 600

int main(int argc, char **argv) {
  int n = argc > 1 ? atoi(argv[1]) : 0;
  grid_init(0.26, 42);
  sim::init(n);
  /* 16 injections of 1000 cells, spread over the grid, as in the A1 bench */
  for (int i = 0; i < 16; i++)
    sim::add_injection(128 + (i % 4) * 256, 96 + (i / 4) * 192, 1000);
  for (int s = 0; s < STEPS; s++)
    sim::step();
  printf("%2d threads  %7.1f million computations/s  %.2f ms/step\n",
         sim::threads(), sim::throughput() / 1e6, sim::step_ms());
  return 0;
}
