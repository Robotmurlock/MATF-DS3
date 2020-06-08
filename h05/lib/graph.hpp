#include <iostream>
#include <vector>
#include <stack>
#include <utility>

unsigned coords_to_index(unsigned i, unsigned j, unsigned m);
std::pair<unsigned, unsigned> index_to_coords(unsigned index, unsigned m);

class Graph{
private:
	std::vector<std::vector<int> > adjacency_list;
	unsigned size;
public:
    Graph(unsigned n);
    void add_edge(int a, int b);
    std::vector<int> hamiltonian_cycle(int start, unsigned n, unsigned m);
};
