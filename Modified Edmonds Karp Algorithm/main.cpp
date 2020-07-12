#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>

#define INFINITE INT_MAX

void error(std::string msg)
{
    std::cout << "Error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

struct Graph {
    std::vector<std::vector<int> > flow_matrix;
    int size;

    Graph(int n)
    {
        size = n;
        flow_matrix.resize(n);
        for(int i=0; i<n; i++)
            flow_matrix.at(i).resize(n, 0);
    }

    // Dodavanje grane u graf
    void add_edge(int a, int b, int flow)
    {
        flow_matrix.at(a).at(b) = flow;
    }

    // Ispisivanje grafa
    void print()
    {
        std::cout << "Graph:" << std::endl;
        for(unsigned i=0; i<flow_matrix.size(); i++)
        {
            std::cout << "Node " << i << ": " << std::endl;
            for(unsigned j=0; j<flow_matrix.at(i).size(); j++)
                if(flow_matrix.at(i).at(j) > 0)
                    std::cout << "\t Child: " 
                              << j << " "
                              << "(" << flow_matrix.at(i).at(j) << ")";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Funkcija vraca:
    // true - Ako je pronadjen povecavajuci put
    // false - Ako nije pronadjen povecavajuci put
    bool bfs(std::vector<int>& parent, const std::vector<std::vector<int> >& residual_matrix)
    {
        // niz posecenih cvorova
        std::vector<bool> visited(size, false);
        // red za BFS, L = que
        std::queue<int> que;
        // Postavlja se izvor kao pocetni cvor (s = 0)
        // Odvod je poslednji cvor, t = size-1
        que.push(0);
        while(!que.empty())
        {
            int node = que.front();
            que.pop();

            // Ako je trenutni cvor odvod,
            // onda je povecavajuci put pronadjen
            if(node == size-1)
                return true;

            // Da li je cvor posecen?
            // Ako jeste, onda se ne procesira
            if(visited.at(node))
                continue;
            // Cvor je posecen
            visited.at(node) = true;

            for(int child=0; child<size; child++)
            {
                // Ako je cvor vec posecen, ignorise se
                if(visited.at(child))
                    continue;
                // Da li je grana protocna?
                if(residual_matrix.at(node).at(child) > 0)
                {
                    // Ako jeste, onda se postavlja novi cvor u red
                    que.push(child);
                    parent.at(child) = node;
                }
            }
        }
        
        return false;
    }



    void edmond_karp()
    {
        auto residual_matrix = flow_matrix;
        std::vector<int> parent(size, -1);

        int max_flow = 0;

        while(bfs(parent, residual_matrix))
        {
            // Azuriravnje protoka
            int edge_start = parent.at(size-1);
            int edge_end = size-1;
            int path_max_flow = INFINITE;
            // Na osnovu pamcenih roditelja se moze rekostruisati put
            // od odvoda do izvora ("roditelj" izvora ima vrednost -1)
            std::vector<int> path;
            while(edge_start != -1)
            {
                path.push_back(edge_end);
                edge_end = parent.at(edge_end);
                edge_start = parent.at(edge_start);
            }
            path.push_back(edge_end);
            // Posto su cvorovi postavljani od kraja ka pocetku
            // potrebno je okrenuti put da ide od pocetka ka kraju
            std::reverse(path.begin(), path.end());

            // Odredjuje je se maksimalni protok puta
            for(unsigned i=0; i<path.size()-1u; i++)
            {
                int start = path.at(i);
                int end   = path.at(i+1);
                int flow = residual_matrix.at(start).at(end);
                path_max_flow = std::min(path_max_flow, flow);
            }

            // Prolazi se opet kako bi se azurirao rezidualni graf
            for(unsigned i=0; i<path.size()-1u; i++)
            {
                int start = path.at(i);
                int end   = path.at(i+1);
                residual_matrix.at(start).at(end) -= path_max_flow;
                residual_matrix.at(end).at(start) += path_max_flow;
            }

            max_flow += path_max_flow;
            std::cout << "Augmented path flow: " << path_max_flow << std::endl;
            std::cout << "Augmented path: ";
            for(unsigned i=0; i<path.size()-1; i++)
                std::cout << path.at(i) << " -> ";
            std::cout << path.back() << std::endl << std::endl;
        }

        std::cout << "Max flow: " << max_flow << std::endl;
    }
};

// Ucitavnje grafa iz datoteke
Graph read_graph_from_file(std::string filename)
{
    std::ifstream input(filename);
    if(input.fail())
        error(std::string("File not found!"));

    int n;
    input >> n;

    Graph g(n);
    int a, b, flow;
    while(input >> a >> b >> flow)
    {
        g.add_edge(a, b, flow);
    }

    return g;
}

int main(int argc, char** argv)
{
    if(argc < 2)
        error(std::string("Wrong number of arguments!"));

    Graph graph = read_graph_from_file(std::string(argv[1]));
    graph.print();
    graph.edmond_karp();

    return 0;
}