#include "grid.h"
#include <cstdlib>
#include <cstring>

static uint8_t cells_a[H][W], cells_b[H][W];
static uint32_t med_layer[H][W], absorb_map[H][W];
static uint8_t pixels[H][W][3];

uint8_t (*cur)[W] = cells_a;
uint8_t (*next)[W] = cells_b;
uint32_t (*med)[W] = med_layer;
uint32_t (*absorb)[W] = absorb_map;
uint32_t stamp = 1;
uint8_t (*rgb)[W][3] = pixels;

void grid_reset(void) {
  memset(cells_a, 0, sizeof cells_a);
  memset(cells_b, 0, sizeof cells_b);
  memset(med_layer, 0, sizeof med_layer);
  memset(absorb_map, 0, sizeof absorb_map);
  stamp = 1;
}

void grid_init(double fraction, unsigned seed) {
  grid_reset();
  srand(seed);
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++)
      cur[y][x] = (rand() / (RAND_MAX + 1.0) < fraction) ? CANCER : HEALTHY;
}

void grid_swap(void) {
  uint8_t (*t)[W] = cur;
  cur = next;
  next = t;
}

grid_counts grid_count(void) {
  grid_counts c = {0, 0, 0};
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++) {
      if (med[y][x] == stamp)
        c.medicine++;
      if (cur[y][x] == CANCER)
        c.cancer++;
      else
        c.healthy++;
    }
  return c;
}
