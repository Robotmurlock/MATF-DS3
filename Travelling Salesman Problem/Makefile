PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp tp_solver.o graph.o
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

tp_solver.o: lib/tp_solver.cpp lib/tp_solver.hpp graph.o
	$(CXX) -c $(FLAGS) $< -o $@

graph.o: lib/graph.cpp lib/graph.hpp
	$(CXX) -c $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(PROGRAM) *.o