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
    // Ovog puta koristim matricu incidencije umesto listu incidencije
    // radi lakse brisanja grana
    std::vector<std::vector<bool> > adjacency_matrix;
    int size;
    bool is_undirected;

    Graph(int n, bool u=false)
    {
        size = n;
        adjacency_matrix.resize(n);
        for(int i=0; i<n; i++)
            adjacency_matrix.at(i).resize(n, false);
        is_undirected = u;
    }

    // Dodavanje grane u graf
    void add_edge(int a, int b)
    {
        adjacency_matrix.at(a).at(b) = true;
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
        for(unsigned i=0; i<adjacency_matrix.size(); i++)
        {
            std::cout << "Node " << i << ": " << std::endl;
            for(unsigned j=0; j<adjacency_matrix.at(i).size(); j++)
                if(adjacency_matrix.at(i).at(j))
                    std::cout << "\t Child: " 
                            << j;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Proverava se da li je usmereni graf ima Ojlerov ciklus ili Ojlerov put
    int d_euler_check()
    {
        // broj izlaznih grana
        int out_deegre_cnt = 0;
        // broj ulaznih grana
        int in_deegre_cnt = 0;
        // indeks pocetnog cvora
        int start_index = 0;

        // Usmereni graf ima Ojlerov ciklus ako svaki cvor ima 
        // broj izlaznih grana jednak broju ulaznih grana
        //
        // Usmereni graf ima Ojlerov put ako postoji tacno jedan
        // cvor koji ima jednu vise izlaznu granu od ulaznih (pocetni cvor)
        // i ako postoji tacno jedan cvor
        // koji ima jednu vise ulaznu granu od izlazni (krajnji cvor)

        for(int i=0; i<size; i++)
        {
            int out_degree = 0;
            int in_degree  = 0;
            for(int j=0; j<size; j++)
            {
                // i -> j (izlazna)
                if(adjacency_matrix.at(i).at(j))
                    out_degree++;
                // j -> i (ulazna)
                if(adjacency_matrix.at(j).at(i))
                    in_degree++;
            }
            // da je ulazni cvor?
            if(in_degree+1 == out_degree) 
            {
                out_deegre_cnt++;
                start_index = i;
            }
            // da li je izlazni cvor?
            else if(out_degree+1 == in_degree) 
            {
                in_deegre_cnt++;
            }
            // da li je obican cvor
            else if(out_degree == in_degree)
            {
                continue;
            }
            // cvor ima previse ulazni ili previse izlaznih grana
            else
            {
                std::cout << "Graph does not have Euler's path or circle!" << std::endl;
                return -1;
            }
        }
        // Provera se broj "ulaznih" i "izlaznih" cvorova
        if(in_deegre_cnt == 0 && out_deegre_cnt == 0)
            std::cout << "Graph has Euler's circle!" << std::endl;
        else if(in_deegre_cnt == 1 && out_deegre_cnt == 1)
            std::cout << "Graph has Euler's path!" << std::endl;
        else
        {
            std::cout << "Graph does not have Euler's path or circle!" << std::endl; 
            return -1;
        }
        return start_index;
    }

    int u_euler_check()
    {
        // indeks pocetnog cvora
        int start_index = 0;
        // broj dvorova sa neparnim brojem grana
        int odd_degree_cnt = 0;

        // Neusmereni graf ima Ojlerov ciklus ako je stepen svakog cvora paran
        // Neusmereni graf ima Ojlerov put ako tacno dva cvora imaju neparan stepan

        for(int i=0; i<size; i++)
        {
            int degree = 0;
            for(int j=0; j<size; j++)
            {
                // i <-> j
                if(adjacency_matrix.at(i).at(j))
                    degree++;
            }
            // da li je broj incidentnih grana neparan (da li je neparan stepen)
            if(degree % 2 == 1) 
            {
                odd_degree_cnt++;
                start_index = i;
            }
        }
        // Proverava se broj cvorova neparnog stepena
        if(odd_degree_cnt == 0)
            std::cout << "Graph has Euler's circle!" << std::endl;
        else if(odd_degree_cnt == 2)
            std::cout << "Graph has Euler's path!" << std::endl;
        else
        {
            std::cout << "Graph does not have Euler's path or circle!" << std::endl; 
            return -1;
        }
        return start_index;
    }

    void u_flery()
    {
        // Flerijev algoritam za neusmerene grafove
        // Prvo se proverava da li je graf Ojlerov
        int start_index = u_euler_check();
        if(start_index == -1)
            return;
        std::cout << "Start index: " << start_index << std::endl;

        std::vector<int> result;
        std::stack<int> s;
        s.push(start_index);

        // Flerijev algoritam: 
        // Krecemo od pocetnog cvora i idemo bilo kojim putem,
        // gde se grana brise kada se ona iskoristi
        // Ako cvor nema vise grana, onda ga stavljamo u rezultat
        // i vracamo se nazad preko STEKA (backtracking)
        // Sa steka se skidaju cvorovi koji nemaju vise grana
        // Kada se naidje na cvor koji ima jos grana, onda nastavljamo algoritam
        // Poenta: Kad se prvi put "zakucamo", nasli smo jedan ciklus/put
        // Onda se "vracamo nazad" da obidjemo zaobidjene cikluse
        while(!s.empty())
        {
            int node = s.top();
            bool has_no_children = true;
            for(int i=0; i<size; i++)
            {
                if(!adjacency_matrix.at(node).at(i))
                    continue;
                int child = i;
                // Brisu se grane (neusmerena grana)
                adjacency_matrix.at(node).at(child) = false;
                adjacency_matrix.at(child).at(node) = false;
                // dodaje se cvor
                s.push(child);
                has_no_children = false;
                break;
            }
            // Ako cvor nema dece => brise se
            if(has_no_children)
            {
                result.push_back(node);
                s.pop();
            }
        }
        // rezultat
        for(unsigned i=0; i<result.size(); i++)
            std::cout << result.at(i) << " ";
        std::cout << std::endl;
    }

    void d_flery()
    {
        // Flerijev algoritam za usmerene grafove
        // Prvo se proverava da li je graf Ojlerov
        int start_index = d_euler_check();
        if(start_index == -1)
            return;
        std::cout << "Start index: " << start_index << std::endl;

        // Jedina razlika u odnosu na neusmereni je nacin brisanja grana

        std::vector<int> result;
        std::stack<int> s;
        s.push(start_index);
        while(!s.empty())
        {
            int node = s.top();
            bool has_no_children = true;
            for(int i=0; i<size; i++)
            {
                if(!adjacency_matrix.at(node).at(i))
                    continue;
                int child = i;
                adjacency_matrix.at(node).at(child) = false;
                s.push(child);
                has_no_children = false;
                break;
            }
            if(has_no_children)
            {
                result.push_back(node);
                s.pop();
            }
        }
        for(unsigned i=0; i<result.size(); i++)
            std::cout << result.at(i) << " ";
        std::cout << std::endl;
    }

    void flery() 
    {
        if(is_undirected)
            u_flery();
        else
            d_flery();
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

    Graph g(n, is_undirected);
    int a, b;
    while(input >> a >> b)
    {
        if(is_undirected)
            g.add_undirected_edge(a, b);
        else
            g.add_edge(a, b);
    }

    return g;
}

int main(int argc, char** argv)
{
    if(argc < 2)
        error(std::string("Wrong number of arguments!"));

    Graph graph = read_graph_from_file(std::string(argv[1]));
    graph.print();
    graph.flery();

    return 0;
}