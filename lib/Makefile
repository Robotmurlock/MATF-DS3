PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp matrix.o
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

matrix.o: matrix.cpp matrix.hpp
	$(CXX) -c $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(PROGRAM)