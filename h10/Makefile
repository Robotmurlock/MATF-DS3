PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp simplex.o matrix.o
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

simplex.o: simplex.cpp simplex.hpp ../lib/matrix.hpp
	$(CXX) -c $(FLAGS) $< -o $@

matrix.o: ../lib/matrix.cpp ../lib/matrix.hpp
	$(CXX) -c $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(PROGRAM) *.o