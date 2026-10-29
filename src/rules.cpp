#include "rules.h"
#include "grid.h"

/* Count the 8 neighbours of (x, y) in grid g equal to value. The loop runs
 * over the clamped 3x3 box including the centre, then the centre is removed. */
template <typename T>
static inline int count_neighbours(T (*g)[W], int x, int y, T value) {
  int x0 = x > 0 ? x - 1 : 0, x1 = x < W - 1 ? x + 1 : W - 1;
  int y0 = y > 0 ? y - 1 : 0, y1 = y < H - 1 ? y + 1 : H - 1;
  int n = 0;
  for (int j = y0; j <= y1; j++)
    for (int i = x0; i <= x1; i++)
      n += (g[j][i] == value);
  return n - (g[y][x] == value);
}

/* Rule 2, second half: the medicine around a cured cell is used up.
 * Two cures can share a neighbour, so two tasks may store here at once.
 * They store the same value, so a relaxed atomic store is enough. */
static inline void absorb_neighbours(int x, int y) {
  int x0 = x > 0 ? x - 1 : 0, x1 = x < W - 1 ? x + 1 : W - 1;
  int y0 = y > 0 ? y - 1 : 0, y1 = y < H - 1 ? y + 1 : H - 1;
  for (int j = y0; j <= y1; j++)
    for (int i = x0; i <= x1; i++)
      __atomic_store_n(&absorb[j][i], stamp, __ATOMIC_RELAXED);
}

void rules_rows(int y0, int y1) {
  for (int y = y0; y < y1; y++)
    for (int x = 0; x < W; x++) {
      uint8_t c = cur[y][x];
      if (c == HEALTHY && count_neighbours(cur, x, y, (uint8_t)CANCER) >= 6)
        c = CANCER;
      else if (c == CANCER && count_neighbours(med, x, y, stamp) >= 6) {
        c = HEALTHY;
        absorb_neighbours(x, y);
      }
      next[y][x] = c;

      uint8_t *p = rgb[y][x];
      if (med[y][x] == stamp) {
        p[0] = 255;
        p[1] = 255;
        p[2] = 0;
      } /* yellow */
      else if (c == CANCER) {
        p[0] = 255;
        p[1] = 0;
        p[2] = 0;
      } /* red */
      else {
        p[0] = 0;
        p[1] = 255;
        p[2] = 0;
      } /* green */
    }
}
