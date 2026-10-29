#ifndef SIM_H
#define SIM_H
#include "medicine.h"
#include <vector>

/* The TBB side of the program: the parallel work of one time step and the
 * throughput counters. The only module that includes TBB headers is sim.cpp. */
namespace sim {

extern std::vector<injection> injections;

void init(int threads); /* threads <= 0: one per processor */
void add_injection(int cx, int cy, int n);
void step(); /* one time step: injection tasks, then rules */

int threads();
double throughput(); /* computations / second of step time */
double step_ms();    /* mean time of one step */

} // namespace sim
#endif
