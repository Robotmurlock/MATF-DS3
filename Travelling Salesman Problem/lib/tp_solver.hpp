#ifndef __TP_SOLVER__
#define __TP_SOLVER__
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
#include "graph.hpp"

#define INF std::numeric_limits<double>::infinity()

#define BAR "*********************************************************************\n"

#define EPS (0.000001)
#define STOP ((unsigned)(-1))

void show_system(
    const std::vector<std::vector<double> >& c, 
    const std::vector<double>& a, 
    const std::vector<double>& b,
    const std::set<unsigned>& pseudo_rows, 
    const std::set<unsigned>& pseudo_columns
);

// support struct for function: "calculate_system_base"
struct cell {
    unsigned i, j;
    double value;

    cell(unsigned x, unsigned y, double v);
};

bool operator>(const cell& c1, const cell& c2);

std::vector<std::vector<std::pair<double, bool> > >
    calculate_system_base(
        const std::vector<std::vector<double> >& c, 
        std::vector<double> a, 
        std::vector<double> b
    );

void show_base_matrix(const std::vector<std::vector<std::pair<double, bool> > >& base_matrix);

std::tuple<bool, unsigned> 
    find_starting_potential(std::vector<std::vector<std::pair<double, bool> > >& base_matrix);

std::tuple<std::vector<double>, std::vector<double> > 
    calculate_potentials(bool row_col, unsigned row_col_index,
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
        const std::vector<std::vector<double> >& c, 
        const std::vector<double>& a, 
        const std::vector<double>& b
    );

void show_potential_vectors(const std::vector<double>& u, const std::vector<double>& v);

std::tuple<unsigned, unsigned> 
    find_theta_start(
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
        const std::vector<std::vector<double> >& c, 
        const std::vector<double>& u, 
        const std::vector<double>& v
    );

void show_theta_start(unsigned theta_i, unsigned theta_j);

std::tuple<std::vector<int>, double>
    find_cycle(
        unsigned theta_i, unsigned theta_j,
        const std::vector<std::vector<std::pair<double, bool> > >& base_matrix
    );

void show_cycle_and_theta(const std::vector<int>& cycle, double theta, unsigned m);

void update_system(
    std::vector<std::vector<std::pair<double, bool> > >& base_matrix, 
    std::vector<int> cycle, double theta,
    unsigned theta_i, unsigned theta_j
);

double calculate_solution(
    std::vector<std::vector<double> >& c, 
    const std::vector<std::vector<std::pair<double, bool> > >& base_matrix,
    const std::set<unsigned>& pseudo_rows, 
    const std::set<unsigned>& pseudo_columns
);

std::tuple<std::set<unsigned>, std::set<unsigned> >
    add_pseudo_vars(
        std::vector<std::vector<double> >& c, 
        std::vector<double>& a, 
        std::vector<double>& b
    );

void show_pseudo_vars(
    const std::set<unsigned>& pseudo_rows, 
    const std::set<unsigned>& pseudo_columns
);

std::pair<double, std::vector<std::vector<std::pair<double, bool> > > > solve_transport_problem(
    std::vector<std::vector<double> >& c, 
    std::vector<double>& a, 
    std::vector<double>& b 
);

#endif