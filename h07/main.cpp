#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <queue>

void error(std::string msg)
{
    std::cout << "Error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

// Pomocna struktura za efikasno odredjivanje i menjanje indeksa K iz koraka 3
// Indeks odgovara komponenti povezanosti
struct UnionFind {
    std::vector<int> parent;
    std::vector<int> height;
    int size;

    // inicijalno svaki cvor ima svoj indeks tj.
    // cvor 0 ima indeks 0
    // cvor 1 ima indeks 1
    // ...
    UnionFind(int n)
    {
        size = n;
        parent.resize(n);
        for(int i=0; i<n; i++)
            parent.at(i) = i;
        height.resize(n, 1);
    }

    // Odredjuje kojeg je indeksa cvor k tj. kom skupu pripada
    int find(int k)
    {
        while(parent.at(k) != k)
            k = parent.at(k);
        return k;
    }

    // Spaja dva indeksa (skupa) u jedan
    void unify(int a, int b)
    {
        int ka = find(a);
        int kb = find(b);
        if(height.at(ka) < height.at(kb))
        {
            parent.at(ka) = kb;
        }
        else if(height.at(ka) > height.at(kb))
        {
            parent.at(kb) = ka;
        }
        else
        {
            parent.at(ka) = kb;
            height.at(kb)++;
        }
    }
};

// Pomocna struktura za cuvanje grana
struct Edge {
    int a, b, w;
    Edge(int _a, int _b, int _w)
        :a(_a), b(_b), w(_w)
        {}
};

// Grane sa vecom tezinom imaju manji prioritet
bool operator<(const Edge& a, const Edge& b)
{
    return a.w > b.w;
}

struct Graph {
    std::vector<std::vector<std::pair<int, int> > > adjacency_list;
    int size;

    Graph(int n)
    {
        size = n;
        adjacency_list.resize(n);
    }

    // Dodavanje grane u graf
    void add_edge(int a, int b, int w)
    {
        // Neusmereni graf, pa je grana kao dve usmerene
        adjacency_list.at(a).push_back(std::make_pair(b, w));
        adjacency_list.at(b).push_back(std::make_pair(a, w));
    }

    // Ispisivanje grafa
    void print()
    {
        std::cout << "Graph:" << std::endl;
        for(unsigned i=0; i<adjacency_list.size(); i++)
        {
            std::cout << "Node " << i << ": " << std::endl;
            for(unsigned j=0; j<adjacency_list.at(i).size(); j++)
                std::cout << "\t Child: " 
                          << adjacency_list.at(i).at(j).first 
                          << ", Weight: "
                          << adjacency_list.at(i).at(j).second << std::endl;
            std::cout << std::endl;
        }
    }

    Graph kruskal()
    {
        // Red sa prioritetom se koristi da se izabere sledeca stranica po redu
        // Odgovara sortiranom nizu (po tezini) grana
        std::priority_queue<Edge> edges;
        for(unsigned i=0; i<adjacency_list.size(); i++)
        {
            int node = i;
            for(unsigned j=0; j<adjacency_list.at(i).size(); j++)
            {
                int child = adjacency_list.at(i).at(j).first;
                int weight = adjacency_list.at(i).at(j).second;
                edges.push(Edge(node, child, weight));
            }
        }

        // Generise se graf sa SIZE cvorova i 0 grana
        Graph tree(size);
        // Indeks struktura
        UnionFind k(size);

        while(!edges.empty())
        {
            // Uzimaju se redom grane
            int a = edges.top().a;
            int b = edges.top().b;
            int w = edges.top().w;
            edges.pop();

            // Ako im indeks nije razlicit onda se ta grana ignorise
            if(k.find(a) == k.find(b))
                continue;

            // Dodaje se grana u novi graf i azuriraju se indeksi
            k.unify(a, b);
            tree.add_edge(a, b, w);
        }

        return tree;
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
    int a, b, w;
    while(input >> a >> b >> w)
        g.add_edge(a, b, w);
    return g;
}

// Cuvanje grafa u datoteku
void save_graph_to_file(const Graph& graph, std::string filename)
{
    std::ofstream output(filename);

    output << graph.size << std::endl;
    for(unsigned i=0; i<graph.adjacency_list.size(); i++)
    {
        int node = i;
        for(unsigned j=0; j<graph.adjacency_list.at(i).size(); j++)
        {
            int child = graph.adjacency_list.at(i).at(j).first;
            int weight = graph.adjacency_list.at(i).at(j).second;
            // Ne upisuju se duplikati
            if(node <= child)
                output << node << " " << child << " " << weight << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
        error(std::string("Wrong number of arguments!"));

    Graph graph = read_graph_from_file(std::string(argv[1]));
    graph.print();
    Graph tree = graph.kruskal();
    tree.print();
    save_graph_to_file(tree, "output.txt");

    return 0;
}