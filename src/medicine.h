
#ifndef MEDICINE_H
#define MEDICINE_H
#include <cstdint>
#include <vector>

struct particle {
  float x, y;   /* position */
  float dx, dy; /* unit direction away from the injection centre */
  bool alive;
};

struct injection {
  int cx, cy;
  std::vector<particle> p;

  /* Fill a disc of n medicine cells around (cx, cy). */
  void init(int cx, int cy, int n);

  /* Move the particles [i0, i1): remove those used up by a cure in the
   * previous step (absorb == prev) or leaving the grid, move the rest one
   * cell outward, stamp their new position in the medicine layer. */
  void move(size_t i0, size_t i1, uint32_t prev);

  int alive() const;
};

#endif
