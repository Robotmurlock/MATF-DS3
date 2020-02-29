PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp matrix.o
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

matrix.o: ../lib/matrix.cpp ../lib/matrix.hpp
	$(CXX) -c $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(PROGRAM) *.o