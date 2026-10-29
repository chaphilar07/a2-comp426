#!/bin/sh
# Usage: ./demo.sh <scenario number>   (close the window to end a scenario)
case "$1" in
1)
  echo "1: four injections, all cores"
  ./cellsim
  ;;
2)
  echo "2: same four injections, 1 thread - compare ms/step with scenario 1"
  ./cellsim -t 1
  ;;
3)
  echo "3: 300, 1500 and 5000 cells - nested parallel_for over particles"
  ./cellsim 200,384,300 500,384,1500 820,384,5000
  ;;
4)
  echo "4: two overlapping injections - two tasks write the same cells"
  ./cellsim 480,384,2000 540,384,2000
  ;;
5)
  echo "5: injections in the corners - medicine leaves the grid"
  ./cellsim 5,5,2000 1020,760,2000 5,760,2000 1020,5,2000
  ;;
6)
  echo "6: sixteen injections on 4 threads - more tasks than threads"
  ./cellsim -t 4 \
    128,96,800 384,96,800 640,96,800 896,96,800 \
    128,288,800 384,288,800 640,288,800 896,288,800 \
    128,480,800 384,480,800 640,480,800 896,480,800 \
    128,672,800 384,672,800 640,672,800 896,672,800
  ;;
7)
  echo "7: rule tests, 24 threads == 1 thread"
  make test
  ;;
8)
  echo "8: every step checked against a sequential reference"
  make ordertest
  ;;
9)
  echo "9: same test with the join removed - must FAIL"
  make brokentest
  ;;
10)
  echo "10: throughput vs threads"
  make bench
  ;;
*) echo "usage: $0 <1-10>" ;;
esac
