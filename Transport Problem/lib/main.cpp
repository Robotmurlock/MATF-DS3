#include "graph.hpp"

int main()
{
	Graph g(5);
	
	g.add_edge(0, 1);
	g.add_edge(0, 3);
	
	g.add_edge(1, 0);
	g.add_edge(1, 2);
	g.add_edge(1, 3);
	g.add_edge(1, 4);
	
	g.add_edge(2, 1);
	g.add_edge(2, 4);
	
	g.add_edge(3, 0);
	g.add_edge(3, 1);
	g.add_edge(3, 4);
	
	g.add_edge(4, 1);
	g.add_edge(4, 2);
	g.add_edge(4, 3);
	
	auto result = g.hamiltonian_cycle(0);
	for(auto r: result)
		std::cout << r << " ";
	std::cout << std::endl;
	
	return 0;
}
