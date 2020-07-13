#include "graph.hpp"

#define NONE   0
#define COLUMN 1
#define ROW    2
#define UNUSED_VAR(x) ((void)x)

unsigned coords_to_index(unsigned i, unsigned j, unsigned m)
{
    return i*m + j;
}

std::pair<unsigned, unsigned> index_to_coords(unsigned index, unsigned m)
{
    return std::make_pair(index / m, index % m);
}

Graph::Graph(unsigned n)
{
    size = n;
    adjacency_list.resize(size);
}

void Graph::add_edge(int a, int b)
{
    adjacency_list.at(a).push_back(b);
}

bool is_stable_path(const std::vector<int>& path, unsigned n, unsigned m)
{
    UNUSED_VAR(n);
    int last_state = NONE;
    int current_state = NONE;
    for(unsigned i=1; i<path.size(); i++)
    {
        auto[x1, y1] = index_to_coords(path.at(i-1), m);
        auto[x2, y2] = index_to_coords(path.at(i), m);

        if(x1 == x2)
            current_state = ROW;
        else if(y1 == y2)
            current_state = COLUMN;
        else
            return false;

        if(current_state == last_state)
            return false;
        last_state = current_state;
    }

    auto[x1, y1] = index_to_coords(path.at(path.size()-1), m);
    auto[x2, y2] = index_to_coords(path.at(0), m);
    if(x1 == x2)
        current_state = ROW;
    else if(y1 == y2)
        current_state = COLUMN;
    else 
        return false;

    if(current_state == last_state)
        return false;

    return true;
}
	
std::vector<int> Graph::cycle(int start, unsigned t_n, unsigned t_m) {
    std::vector<int> parent(size, -1);
    std::vector<bool> on_stack(size, false);

    std::stack<std::pair<int, int> > s;
    s.push(std::make_pair(start, 0));
    on_stack.at(start) = true;
    
    std::vector<int> path;
    
    while(!s.empty())
    {
        int node = s.top().first;
        unsigned index = s.top().second;

        if(index == adjacency_list.at(node).size())
        {
            s.pop();
            on_stack.at(node) = false;
            parent.at(node) = -1;
            continue;
        }
        
        s.top().second++;
        
        int child = adjacency_list.at(node).at(index);
        if(child == start && parent.at(node) != start)
        {
            if(s.size() % 2 == 0)
            {
                path.push_back(start);
                int n = node;
                while(n != -1)
                {
                    path.push_back(n);
                    n = parent.at(n);
                }


                if(is_stable_path(path, t_n, t_m))
                    return path;
                else
                    path.resize(0);
            }
        }
    
        if(!on_stack.at(child))
        {
            s.push(std::make_pair(child, 0));
            parent.at(child) = node;
            on_stack.at(child) = true;
        }
    }
    
    return std::vector<int>();
}