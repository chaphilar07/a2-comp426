
#ifndef GRID_H
#define GRID_H
#include <cstdint>

#define W 1024
#define H 768

enum cell_type : uint8_t { HEALTHY = 0, CANCER = 1 };

extern uint8_t (*cur)[W];
extern uint8_t (*next)[W];
extern uint32_t (*med)[W];
extern uint32_t (*absorb)[W];
extern uint32_t stamp;
extern uint8_t (*rgb)[W][3];

struct grid_counts {
  long healthy, cancer, medicine;
};

void grid_init(double cancer_fraction, unsigned seed);
void grid_reset(void);
void grid_swap(void);
grid_counts grid_count(void);
unsigned long long grid_checksum(void);

#endif
