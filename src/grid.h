#ifndef GRID_H
#define GRID_H
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define W 1024
#define H 768

enum cell_type : uint8_t { HEALTHY = 0, CANCER = 1 };

static uint8_t cells_a[H][W], cells_b[H][W];
static uint32_t med_layer[H][W], absorb_map[H][W];
static uint8_t pixels[H][W][3];

extern uint8_t (*cur)[W];
extern uint8_t (*next)[W];

extern uint8_t (*med)[W];
extern uint8_t (*absorb)[W];
uint32_t stamp = 1;
uint8_t (*rgb)[W][3] = pixels;

struct grid_counts {
  long healthy, cancer, medicine;
};

#endif
