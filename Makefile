CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra
LDLIBS   = -ltbb -lglut -lGLU -lGL -lm
SRC      = $(wildcard src/*.cpp)
OBJ      = $(SRC:.cpp=.o)
SIMSRC   = src/grid.cpp src/rules.cpp src/medicine.cpp src/sim.cpp

cellsim: $(OBJ)
				$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

test: tests/test_sim.cpp $(SIMSRC)
				$(CXX) $(CXXFLAGS) -Isrc -o test_sim $^ -ltbb -lm
				./test_sim

ordertest: tests/test_order.cpp $(SIMSRC)
				$(CXX) $(CXXFLAGS) -Isrc -o test_order $^ -ltbb -lm
				./test_order

brokentest: tests/test_order.cpp $(SIMSRC)
				$(CXX) $(CXXFLAGS) -DBROKEN_NO_JOIN -Isrc -o test_broken $^ -ltbb -lm
				./test_broken

bench: tests/bench.cpp $(SIMSRC)
				$(CXX) $(CXXFLAGS) -Isrc -o bench_sim $^ -ltbb -lm
				for n in 1 2 4 8 16 24; do ./bench_sim $$n; done

clean:
				rm -f $(OBJ) cellsim test_sim test_order test_broken bench_sim

.PHONY: test ordertest brokentest bench clean
