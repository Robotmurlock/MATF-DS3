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

void error(std::string msg)
{
    std::cout << "Error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

struct Graph {
    std::vector<std::vector<int> > adjacency_list;
    int size;

    Graph(int n)
    {
        size = n;
        adjacency_list.resize(n);
    }

    // Dodavanje grane u graf
    void add_edge(int a, int b)
    {
        adjacency_list.at(a).push_back(b);
    }

    // Dodavanje neusmerene grane u graf
    void add_undirected_edge(int a, int b)
    {
        add_edge(a, b);
        add_edge(b, a);
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
                          << adjacency_list.at(i).at(j);
            std::cout << std::endl;
        }
    }

    void dfs(int start=0) 
    {
        // Za svaki cvor se sortiraju (opadajuce) indeksi suseda
        // kako bi koriscenjem STACK (FILO ~ First In Last Out) strukture indeksi bili 
        // birani u rastucem redosledu
        for(int i=0; i<size; i++)
        {
            std::sort(adjacency_list.at(i).begin(), adjacency_list.at(i).end());
            std::reverse(adjacency_list.at(i).begin(), adjacency_list.at(i).end());
        }

        std::cout << "DFS: ";
        // Koristi se pomocni niz koji sluzi da se oznace poseceni cvorovi
        // i STACK struktura
        std::vector<bool> visited(size, false);
        std::stack<int> s;
        // Ubacuje se pocetni cvor
        s.push(start);

        // Kada se stek isprazni, svi cvorovi su poseceni i obilazak je zavrsen
        while(!s.empty())
        {
            // Uzima se cvor sa vrha steka
            int node = s.top();
            // Brise se cvor sa vrha steka
            s.pop();
            // Ako je taj cvor vec posecen, on se ignorise
            if(visited.at(node))
                continue;
            // Postavlja se da je cvor posecen
            visited.at(node) = true;

            std::cout << node << " ";

            // Svi susedi cvora se postavljaju na steku tako sto se prvo
            // na stek postavlja cvor sa najvecim indeksom, pa onda oni sa manjim
            // Ovim se postize da se na vrhu steka nalazi cvor sa najmanjim indeksom
            for(unsigned i=0; i<adjacency_list.at(node).size(); i++)
            {
                int child = adjacency_list.at(node).at(i);
                s.push(child);
            }
        }
        std::cout << std::endl;
    }

    void bfs(int start=0) 
    {
        // Za svaki cvor se sortiraju (rastuce) indeksi suseda
        // kako bi koriscenjem reda, QUEUE (FIFO ~ First In First Out) strukture indeksi bili 
        // birani u rastucem redosledu
        for(int i=0; i<size; i++)
            std::sort(adjacency_list.at(i).begin(), adjacency_list.at(i).end());

        std::cout << "BFS: ";
        // Koristi se pomocni niz koji sluzi da se oznace poseceni cvorovi
        // i QUEUE struktura
        std::vector<bool> visited(size, false);
        std::queue<int> s;
        // Ubacuje se pocetni cvor
        s.push(start);

        while(!s.empty())
        {
            // Uzima se cvor koji je sledeci po redu
            int node = s.front();
            // Brise se cvor sa pocetka reda
            s.pop();
            // Ako je taj cvor vec posecen, on se ignorise
            if(visited.at(node))
                continue;
            // Postavlja se da je cvor posecen
            visited.at(node) = true;

            std::cout << node << " ";

            // Svi susedi cvora se postavljaju u red tako sto se
            // u red se postavlja cvor sa najmanjim indeksom, pa onda oni sa vecim
            // Ovim se postize da se na pocetku reda nalazi cvor sa najmanjim indeksom
            for(unsigned i=0; i<adjacency_list.at(node).size(); i++)
            {
                int child = adjacency_list.at(node).at(i);
                s.push(child);
            }
        }
        std::cout << std::endl;
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
    std::string type;
    input >> type;
    bool is_undirected = (type == "u");

    Graph g(n);
    int a, b;
    while(input >> a >> b)
        if(is_undirected)
            g.add_undirected_edge(a, b);
        else
            g.add_edge(a, b);

    return g;
}

int main(int argc, char** argv)
{
    if(argc < 2)
        error(std::string("Wrong number of arguments!"));

    Graph graph = read_graph_from_file(std::string(argv[1]));
    graph.print();
    graph.dfs();
    graph.bfs();

    return 0;
}