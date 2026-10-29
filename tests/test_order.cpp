
#include "grid.h"
#include "medicine.h"
#include "sim.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#define N_INJ 16
#define STEPS 300

static uint8_t old_cells[H][W];
static uint32_t old_absorb[H][W];
static uint8_t want_med[H][W];

static int count_cells(int x, int y) {
  int n = 0;
  for (int j = y - 1; j <= y + 1; j++)
    for (int i = x - 1; i <= x + 1; i++)
      if (i >= 0 && i < W && j >= 0 && j < H && !(i == x && j == y))
        n += old_cells[j][i] == CANCER;
  return n;
}
static int count_med(int x, int y) {
  int n = 0;
  for (int j = y - 1; j <= y + 1; j++)
    for (int i = x - 1; i <= x + 1; i++)
      if (i >= 0 && i < W && j >= 0 && j < H && !(i == x && j == y))
        n += want_med[j][i];
  return n;
}

int main(void) {
  long bad_medicine = 0, bad_layer = 0, bad_rules = 0;

  grid_init(0.26, 42);
  sim::init(0);
  for (int i = 0; i < N_INJ; i++)
    sim::add_injection(392 + (i % 4) * 80, 264 + (i / 4) * 80, 1500);

  std::vector<std::vector<particle>> old_p(N_INJ);

  for (int s = 0; s < STEPS; s++) {
    /* remember the state before the step; absorb must be copied because
     * this step's rules overwrite it where a cell is absorbed again */
    memcpy(old_cells, cur, sizeof old_cells);
    memcpy(old_absorb, absorb, sizeof old_absorb);
    for (int a = 0; a < N_INJ; a++)
      old_p[a] = sim::injections[a].p;
    uint32_t prev = stamp;

    sim::step();

    /* 1. medicine */
    memset(want_med, 0, sizeof want_med);
    for (int a = 0; a < N_INJ; a++)
      for (size_t i = 0; i < old_p[a].size(); i++) {
        particle b = old_p[a][i], q = sim::injections[a].p[i];
        int nx = (int)std::floor(b.x + b.dx), ny = (int)std::floor(b.y + b.dy);
        int gone = !b.alive || old_absorb[(int)b.y][(int)b.x] == prev ||
                   nx < 0 || nx >= W || ny < 0 || ny >= H;
        if (gone) {
          if (q.alive)
            bad_medicine++;
          continue;
        }
        if (!q.alive || q.x != b.x + b.dx || q.y != b.y + b.dy)
          bad_medicine++;
        want_med[ny][nx] = 1;
      }
    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
        if ((med[y][x] == stamp) != want_med[y][x])
          bad_layer++;

    /* 2. rules, on the old cells and the new medicine */
    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++) {
        uint8_t c = old_cells[y][x], want = c;
        if (c == HEALTHY && count_cells(x, y) >= 6)
          want = CANCER;
        else if (c == CANCER && count_med(x, y) >= 6)
          want = HEALTHY;
        if (cur[y][x] != want)
          bad_rules++;
      }
  }

  printf("%s  medicine moved or removed correctly   (%ld wrong)\n",
         bad_medicine ? "FAIL" : "PASS", bad_medicine);
  printf("%s  medicine layer matches live particles (%ld wrong positions)\n",
         bad_layer ? "FAIL" : "PASS", bad_layer);
  printf("%s  rules read old cells and new medicine  (%ld wrong cells)\n",
         bad_rules ? "FAIL" : "PASS", bad_rules);
  printf("checksum %016llx   (%d threads)\n", grid_checksum(), sim::threads());
  return (bad_medicine || bad_layer || bad_rules) ? 1 : 0;
}
