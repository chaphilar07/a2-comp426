
#include "medicine.h"
#include "grid.h"
#include <cmath>

void injection::init(int x0, int y0, int n) {
  cx = x0;
  cy = y0;
  p.clear();
  p.reserve(n);
  int r = (int)std::ceil(std::sqrt(n / M_PI));
  for (int i = cx - r; i <= cx + r && (int)p.size() < n; i++) {
    if (i < 0 || i >= W)
      continue;
    for (int j = cy - r; j <= cy + r && (int)p.size() < n; j++) {
      if (j < 0 || j >= H)
        continue;
      int ox = i - cx, oy = j - cy;
      if (ox * ox + oy * oy > r * r)
        continue;
      float len = std::sqrt((float)(ox * ox + oy * oy));
      particle q;
      q.dx = len == 0 ? 1 : ox / len;
      q.dy = len == 0 ? 0 : oy / len;
      q.x = i;
      q.y = j;
      q.alive = true;
      med[j][i] = stamp;
      cur[j][i] = HEALTHY;
      p.push_back(q);
    }
  }
}

void injection::move(size_t i0, size_t i1, uint32_t prev) {
  for (size_t i = i0; i < i1; i++) {
    particle &q = p[i];
    if (!q.alive)
      continue;
    if (absorb[(int)q.y][(int)q.x] == prev) { /* used up by a cure last step */
      q.alive = false;
      continue;
    }
    float nx = q.x + q.dx, ny = q.y + q.dy;
    int ix = (int)std::floor(nx), iy = (int)std::floor(ny);
    if (ix < 0 || ix >= W || iy < 0 || iy >= H) { /* left the grid */
      q.alive = false;
      continue;
    }
    q.x = nx;
    q.y = ny;
    /* Two injections can land on the same cell. Both store the same stamp,
     * so a relaxed atomic store is enough; no lock. */
    __atomic_store_n(&med[iy][ix], stamp, __ATOMIC_RELAXED);
  }
}

int injection::alive() const {
  int a = 0;
  for (const particle &q : p)
    a += q.alive;
  return a;
}
