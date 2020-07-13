#include <iostream>
#include <string>
#include "lib/tp_solver.hpp"

#define PSEUDO_INF (1000000000.0)

std::tuple<std::vector<std::vector<double> >, std::vector<double>, std::vector<double> >
    read_task(std::ifstream& input)
{
    unsigned n;
    input >> n;

    std::vector<std::vector<double> > c(n, std::vector<double>(n));
    std::vector<double> a(n, 1.0), b(n, 1.0);
    std::string next;

    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<n; j++)
        {
            input >> next;
            if(next == "*")
                c.at(i).at(j) = PSEUDO_INF;
            else
                c.at(i).at(j) = std::stoi(next);
        }
    }

    return std::make_tuple(c, a, b);
}

std::vector<int> get_cycle(const std::vector<std::vector<std::pair<double, bool> > >& solution)
{
    std::vector<int> cycle;
    unsigned n = solution.size();

    std::vector<bool> visited(n, false);
    cycle.push_back(0);
    visited.at(0) = true;
    while(true)
    {
        for(unsigned i=0; i<n; i++)
        {
            if(solution.at(cycle.back()).at(i).first >= EPS)
            {
                if(visited.at(i))
                    return cycle;
                visited.at(i) = true;
                cycle.push_back(i);
                break;
            }
        }
    }

    return cycle;
}

double record = PSEUDO_INF;
std::vector<int> record_cycle;
void solve(std::vector<std::vector<double> >& c, std::vector<double>& a, std::vector<double>& b)
{
    unsigned n = a.size();
    auto[value, solution] = solve_transport_problem(c, a, b);
    if(value > record)
        return;

    auto cycle = get_cycle(solution);
    if(cycle.size() == n)
    {
        record = value;
        record_cycle = cycle;
        return;
    }
    std::cout << "[TSP] ~ cycle: ";
    for(auto c: cycle)
        std::cout << c << " ";
    std::cout << std::endl;

    for(unsigned i=0; i<cycle.size()-1; i++)
    {
        int start = cycle.at(i);
        int end   = cycle.at(i+1);
        double tmp = c.at(start).at(end);
        c.at(start).at(end) = PSEUDO_INF;
        solve(c, a, b);
        c.at(start).at(end) = tmp;
    }
    double tmp = c.at(cycle.back()).at(cycle.at(0));
    c.at(cycle.back()).at(cycle.at(0)) = PSEUDO_INF;
    solve(c, a, b);
    c.at(cycle.back()).at(cycle.at(0)) = tmp;
}

int main(int argc, char** argv)
{
    // *INPUT FILE*
    const char* path = (argc >= 2) ? argv[1] : "input.txt";

    std::ifstream input(path);
    if(input.fail())
    {
        std::cout << "Failed to open \"input.txt\"!" << std::endl;
        return 1;
    }

    auto[c, a, b] = read_task(input);
    solve(c, a, b);

    std::cout << "Final solution: " << record << std::endl;
    std::cout << "Path: ";
    for(auto c: record_cycle)
        std::cout << c << " ";
    std::cout << std::endl; 

    return 0;
}