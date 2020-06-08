PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp graph.o
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

graph.o: lib/graph.cpp lib/graph.hpp
	$(CXX) -c $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(PROGRAM) *.o