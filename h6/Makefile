PROGRAM = program
CXX = g++
FLAGS = -Wextra -Wall -std=c++17

$(PROGRAM): main.cpp
	$(CXX) $(FLAGS) $^ -o $(PROGRAM)

.PHONY: clean

clean:
	rm $(PROGRAM) *.o