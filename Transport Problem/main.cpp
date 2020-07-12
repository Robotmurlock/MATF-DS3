#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <utility>
#include <algorithm>
#include <tuple>
#include <iomanip>
#include <string>
#include <set>
#include <queue>
#include <cmath>
#include <cfloat>
#include <stack>
#include "lib/graph.hpp"

#define INF std::numeric_limits<double>::infinity()

#define BAR "*********************************************************************\n"

#define EPS (0.000001)
#define STOP ((unsigned)(-1))

std::tuple<std::vector<std::vector<double> >, std::vector<double>, std::vector<double> >
    read_task(std::ifstream& input)
{
    unsigned n, m;

    input >> n >> m;

    std::vector<std::vector<double> > c(n, std::vector<double>(m));
    std::vector<double> a(n), b(m);

    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m; j++)
            input >> c.at(i).at(j);
        input >> a.at(i);
    }
    for(unsigned j=0; j<m; j++)
        input >> b.at(j);

    return std::make_tuple(c, a, b);
}

void show_system(const std::vector<std::vector<double> >& c, 
                 const std::vector<double>& a, 
                 const std::vector<double>& b,
                 const std::set<unsigned>& pseudo_rows, 
                 const std::set<unsigned>& pseudo_columns)
{
    std::cout << "system matrix:" << std::endl;
    auto n = a.size(), m = b.size();
    unsigned dim = 10;
    std::string bar(dim*(n+2)+n+1, '-');

    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m-1; j++)
        {
            std::cout.width(dim);
            if(pseudo_rows.find(i) != pseudo_rows.end() || pseudo_columns.find(j) != pseudo_columns.end())
                std::cout << "*";
            else
                std::cout << c.at(i).at(j);
            std::cout << " ";
        }
        std::cout.width(dim);
        if(pseudo_rows.find(i) != pseudo_rows.end() || pseudo_columns.find(m-1) != pseudo_columns.end())
            std::cout << "*";
        else
            std::cout << c.at(i).at(m-1);
        std::cout.width(dim/2);
        std::cout << "|";
        std::cout.width(dim/2);
        std::cout << a.at(i) << std::endl;
    }
    std::cout << bar << std::endl;
    for(unsigned j=0; j<m; j++)
    {
        std::cout.width(dim);
        std::cout << b.at(j) << " ";
    }
        std::cout << std::endl << std::endl;
    
}

// support struct for function: "calculate_system_base"
struct cell {
    unsigned i, j;
    double value;

    cell(unsigned x, unsigned y, double v)
        : i(x), j(y), value(v)
        {}

    
};

bool operator>(const cell& c1, const cell& c2) { return c1.value > c2.value; }

std::vector<std::vector<std::pair<double, bool> > >
    calculate_system_base(const std::vector<std::vector<double> >& c, 
        std::vector<double> a, 
        std::vector<double> b)
{
    auto n = a.size(), m = b.size();
    std::vector<std::vector<std::pair<double, bool> > > base_matrix (n, std::vector<std::pair<double, bool> >(m, std::make_pair(0.0, false)));

    // We can't use empty_rows or empty_columns because x(i, j) = a(i) or x(i, j) = b(j)
    std::set<unsigned> empty_rows, empty_columns;
    // min heap (priority queue) is used as an optimization for this function for O(n*m*log(n*m)) time complexity
    // instead of O((n+m)*n*m) time complexity
    std::priority_queue<cell, std::vector<cell>, std::greater<cell> > candidates;
    for(unsigned i=0; i<n; i++)
        for(unsigned j=0; j<m; j++)
            candidates.push(cell(i, j, c.at(i).at(j)));

    // used to find starting potential
    unsigned iteration = 0;
    while(iteration < n+m-1)
    {
        int i = candidates.top().i;
        int j = candidates.top().j;
        if(empty_rows.find(i) == empty_rows.end() && empty_columns.find(j) == empty_columns.end())
        {
            double min_ab = std::min(a.at(i), b.at(j));
            base_matrix.at(i).at(j) = std::make_pair(min_ab, true);
            iteration++;
            a.at(i) -= min_ab;
            b.at(j) -= min_ab;

            if(std::fabs(a.at(i)) < EPS)
            {
                empty_rows.insert(i);
            }
            else if(std::fabs(b.at(j)) < EPS)
            {
                empty_columns.insert(j);
            }
        }
        candidates.pop();
    }

    return base_matrix;
}

void show_base_matrix(const std::vector<std::vector<std::pair<double, bool> > >& base_matrix)
{
    std::cout << "base matrix:" << std::endl;
    unsigned n = base_matrix.size(), m = base_matrix.at(0).size();
    unsigned dim = 10;
    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m; j++)
        {
            std::cout.width(dim);
            std::cout << base_matrix.at(i).at(j).first << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m; j++)
        {
            std::cout.width(dim);
            if(base_matrix.at(i).at(j).second)
                std::cout << "#" << " ";
            else
                std::cout << "." << " ";
            
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::tuple<bool, unsigned> find_starting_potential(std::vector<std::vector<std::pair<double, bool> > >& base_matrix)
{
    unsigned n = base_matrix.size(), m = base_matrix.at(0).size();
    std::vector<int> bases_per_row_cnt(n, 0), bases_per_column_cnt(m, 0);

    for(unsigned i=0; i<n; i++)
        for(unsigned j=0; j<m; j++)
            if(base_matrix.at(i).at(j).second)
            {
                bases_per_row_cnt.at(i)++;
                bases_per_column_cnt.at(j)++;
            }

    // true -> row, false -> column
    bool row_col = true;

    int max_cnt = -1;
    unsigned index;
    for(unsigned i=0; i<n; i++)
        if(bases_per_row_cnt.at(i) > max_cnt)
        {
            max_cnt = bases_per_row_cnt.at(i);
            index = i;
            row_col = true;
        }

    for(unsigned j=0; j<m; j++)
        if(bases_per_column_cnt.at(j) > max_cnt)
        {
            max_cnt = bases_per_column_cnt.at(j);
            index = j;
            row_col = false;
        }

    return std::make_tuple(row_col, index);
}

std::tuple<std::vector<double>, std::vector<double> > 
    calculate_potentials(bool row_col, unsigned row_col_index,
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
        const std::vector<std::vector<double> >& c, 
        const std::vector<double>& a, 
        const std::vector<double>& b)
{
    unsigned n = a.size(), m = b.size();
    std::vector<double> u(n), v(m);
    std::set<unsigned> calculated_row, calculated_column;

    if(row_col)
    {
        u.at(row_col_index) = 0.0;
        calculated_row.insert(row_col_index);
    }
    else
    {
        v.at(row_col_index) = 0.0;
        calculated_column.insert(row_col_index);
    }

    // pair<index, row_col>, true -> row, false -> col
    std::stack<std::pair<unsigned, bool> > s;
    s.push(std::make_pair(row_col_index, row_col));
    while(!s.empty())
    {
        unsigned index = s.top().first;
        bool is_row = s.top().second;
        s.pop();

        if(is_row)
        {
            for(unsigned j=0; j<m; j++)
            {
                if(!base_matrix.at(index).at(j).second)
                    continue;

                if(calculated_column.find(j) == calculated_column.end())
                {
                    v.at(j) = c.at(index).at(j) - u.at(index);
                    calculated_column.insert(j);
                    s.push(std::make_pair(j, false));
                }
            }
        }
        else
        {
            for(unsigned i=0; i<n; i++)
            {
                if(!base_matrix.at(i).at(index).second)
                    continue;

                if(calculated_row.find(i) == calculated_row.end())
                {
                    u.at(i) = c.at(i).at(index) - v.at(index);
                    calculated_row.insert(i);
                    s.push(std::make_pair(i, true));
                }
            }
        }
    }

    return std::make_tuple(u, v);
}

void show_potential_vectors(const std::vector<double>& u, const std::vector<double>& v)
{
    std::cout << "potential vectors:" << std::endl;
    std::cout << "u: ";
    for(auto value: u)
        std::cout << value << " ";
    std::cout << std::endl;
    std::cout << "v: ";
    for(auto value: v)
        std::cout << value << " ";
    std::cout << std::endl << std::endl;
}

std::tuple<unsigned, unsigned> find_theta_start(
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
        const std::vector<std::vector<double> >& c, 
        const std::vector<double>& u, 
        const std::vector<double>& v)
{
    unsigned theta_i = STOP;
    unsigned theta_j = STOP;
    double theta_value = INF;

    unsigned n = u.size(), m = v.size();

    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m; j++)
        {
            if(base_matrix.at(i).at(j).second)
                continue;
            
            double p = c.at(i).at(j) - u.at(i) - v.at(j);
            if(p < 0.0)
            {
                if(p < theta_value)
                {
                    theta_value = p;
                    theta_i = i;
                    theta_j = j;
                }
            }
        }
    }

    return std::make_tuple(theta_i, theta_j);
}

void show_theta_start(unsigned theta_i, unsigned theta_j)
{
    std::cout << "theta position: " << std::endl;
    if(theta_i == STOP && theta_j == STOP)
        std::cout << "STOP" << std::endl;
    else
        std::cout << theta_i << " " << theta_j << std::endl;
    std::cout << std::endl;
}

std::tuple<std::vector<int>, double>
    find_cycle(unsigned theta_i, unsigned theta_j,
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix)
{
    unsigned n = base_matrix.size(), m = base_matrix.at(0).size();
    double theta = INF;

    // Creating graph
    Graph g(n*m);

    for(unsigned i=0; i<n; i++)
    {
        for(unsigned j=0; j<m; j++)
        {
            if(!base_matrix.at(i).at(j).second && (i != theta_i || j != theta_j))
                continue;

            for(unsigned k=0; k<n; k++)
            {
                if(i == k)
                    continue;
                if(!base_matrix.at(k).at(j).second && (k != theta_i || j != theta_j))
                    continue;
                g.add_edge(coords_to_index(i, j, m), coords_to_index(k, j, m));
            }

            for(unsigned k=0; k<m; k++)
            {
                if(j == k)
                    continue;
                if(!base_matrix.at(i).at(k).second  && (i != theta_i || k != theta_j))
                    continue;
                g.add_edge(coords_to_index(i, j, m), coords_to_index(i, k, m));
            }
        }
    }

    auto cycle = g.hamiltonian_cycle(coords_to_index(theta_i, theta_j, m), n, m);

    for(unsigned k=0; k<cycle.size(); k++)
    {
        if(k%2 == 0)
            continue;
        
        auto[i, j] = index_to_coords(cycle.at(k), m);
        theta = std::min(theta, base_matrix.at(i).at(j).first);
    }

    return std::make_tuple(cycle, theta);
}

void show_cycle_and_theta(const std::vector<int>& cycle, double theta, unsigned m)
{
    std::cout << "cycle: ";
    for(unsigned i=0; i<cycle.size(); i++)
    {
        unsigned p = cycle.at(i);
        std::cout << "(" << index_to_coords(p, m).first << ", " << index_to_coords(p, m).second << ") ";
    }
    std::cout << std::endl;
    std::cout << "theta: " << theta << std::endl;
    std::cout << std::endl;
}

void update_system(std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
    std::vector<int> cycle, double theta,
    unsigned theta_i, unsigned theta_j)
{
    unsigned m = base_matrix.at(0).size();
    bool not_removed = true;

    for(unsigned k=0; k<cycle.size()-1; k++)
    {
        auto[i, j] = index_to_coords(cycle.at(k), m);
        if(k%2 == 0)
        {
            base_matrix.at(i).at(j).first += theta;
        }
        else 
        {
            base_matrix.at(i).at(j).first -= theta;
            if(std::fabs(base_matrix.at(i).at(j).first) < EPS && not_removed)
            {
                base_matrix.at(i).at(j).second = false;
                not_removed = false;
            }
        }
    }

    base_matrix.at(theta_i).at(theta_j).second = true;
}

double calculate_solution(std::vector<std::vector<double> >& c, 
                          const std::vector<std::vector<std::pair<double, bool> > >& base_matrix,
                          const std::set<unsigned>& pseudo_rows, 
                          const std::set<unsigned>& pseudo_columns)
{
    double result = 0;
    unsigned n = base_matrix.size(), m = base_matrix.at(0).size();

    for(unsigned i=0; i<n; i++)
    {
        if(pseudo_rows.find(i) != pseudo_rows.end())
            continue;
        for(unsigned j=0; j<m; j++)
        {
            if(pseudo_columns.find(j) != pseudo_columns.end())
                continue;
            result += base_matrix.at(i).at(j).first*c.at(i).at(j);
        }
    }
    
    return result;
}

std::tuple<std::set<unsigned>, std::set<unsigned> >
    add_pseudo_vars(std::vector<std::vector<double> >& c, 
        std::vector<double>& a, 
        std::vector<double>& b)
{
    unsigned n = a.size(), m = b.size();
    std::set<unsigned> pseudo_rows, pseudo_columns;

    double sum_a = 0.0, sum_b = 0.0;
    for(auto val: a)
        sum_a += val;
    for(auto val: b)
        sum_b += val;

    double diff = sum_a - sum_b;
    if(std::fabs(diff) < EPS)
        return std::make_tuple(pseudo_rows, pseudo_columns);

    double pseudo_value = -INF;
    for(unsigned i=0; i<n; i++)
        for(unsigned j=0; j<m; j++)
            pseudo_value = std::max(pseudo_value, c.at(i).at(j));

    pseudo_value += 1000000.0;

    if(diff < 0)
    {
        // insert new row
        std::vector<double> new_row(m, pseudo_value);
        c.push_back(new_row);
        a.push_back(std::fabs(diff));
        pseudo_rows.insert(n);
    }
    else
    {
        // insert new column
        for(unsigned i=0; i<n; i++)
        c.at(i).push_back(pseudo_value);
        b.push_back(std::fabs(diff));
        pseudo_columns.insert(m);
    }

    return std::make_tuple(pseudo_rows, pseudo_columns);
}

void show_pseudo_vars(const std::set<unsigned>& pseudo_rows, const std::set<unsigned>& pseudo_columns)
{
    std::cout << "pseudo rows: ";
    for(auto v: pseudo_rows)
        std::cout << v << " ";
    std::cout << std::endl; 
    std::cout << "pseudo columns: ";
    for(auto v: pseudo_columns)
        std::cout << v << " ";
    std::cout << std::endl; 
}

double solve_transport_problem(
    std::vector<std::vector<double> >& c, 
    std::vector<double>& a, 
    std::vector<double>& b 
)
{
    std::cout << BAR << BAR << BAR;
    auto[pseudo_rows, pseudo_columns] = add_pseudo_vars(c, a, b);
    show_pseudo_vars(pseudo_rows, pseudo_columns);
    show_system(c, a, b, pseudo_rows, pseudo_columns);
    // minimal price method:
    auto base_matrix = calculate_system_base(c, a, b);
    show_base_matrix(base_matrix);

    while(true)
    {
        auto[row_col, row_col_index] = find_starting_potential(base_matrix);

        // potential method:
        auto[u, v] = calculate_potentials(row_col, row_col_index, base_matrix, c, a, b);
        show_potential_vectors(u, v);

        // check if task is completed or find "theta start"
        auto[theta_i, theta_j] = find_theta_start(base_matrix, c, u, v);
        show_theta_start(theta_i, theta_j);

        // STOP?
        if(theta_i == STOP && theta_j == STOP)
        {
            const double result = calculate_solution(c, base_matrix, pseudo_rows, pseudo_columns);
            std::cout << "Solution: " << result << std::endl;
            std::cout << BAR << BAR << BAR;
            return result;
        }

        auto[cycle, theta] = find_cycle(theta_i, theta_j, base_matrix);
        show_cycle_and_theta(cycle, theta, b.size()); // (m = b.size())

        update_system(base_matrix, cycle, theta, theta_i, theta_j);
        show_base_matrix(base_matrix);
    }
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
    solve_transport_problem(c, a, b);
    return 0;
}

