#ifndef __SIMPLEX__
#define __SIMPLEX__

#include <iostream>
#include <fstream>
#include <utility>
#include <climits>
#include <cfloat>
#include <cmath>
#include <ctime>
#include <set>
#include <iomanip>
#include <sstream>
#include "../lib/matrix.hpp"

#define UNUSED_VAR(X) ((void)X)

#define STOP ((unsigned)-1)
#define INF DBL_MAX

#define BAR "------------------------------------------------------"

// precision: EPS
#define EPS 0.0001

bool is_not_canonical(const Matrix& A, const Matrix& b);

std::tuple<std::vector<unsigned>, std::vector<unsigned>, double> set_canonical_matrix(Matrix& A, Matrix& b, Matrix& c);

Matrix get_B(const Matrix& A, const std::vector<unsigned>& P);

Matrix get_Cb(const Matrix& c, const std::vector<unsigned>& P);

Matrix get_Kq(const Matrix& A, const std::vector<unsigned>& Q);

Matrix get_Cq(const Matrix& c, const std::vector<unsigned>& Q);

unsigned get_first_negative(const Matrix& r);

Matrix get_x(const Matrix& b, const std::vector<unsigned>& P, unsigned size);

std::pair<double, unsigned> get_t_opt(const Matrix& x, const Matrix& y, const std::vector<unsigned>& P);

void update_x(Matrix& x, const Matrix& y, unsigned l, const std::vector<unsigned>& P, double t_opt);

void update_P_Q(std::vector<unsigned>& P, std::vector<unsigned>& Q, unsigned t_index, unsigned l);

bool has_all_negative(const Matrix& y);

std::tuple<double, Matrix, Matrix, Matrix, Matrix>  
residual_simplex(Matrix& A, Matrix& b, Matrix& c,
                 std::vector<unsigned>& P, std::vector<unsigned>& Q, double Fo);

void show_system(const Matrix& A, const Matrix& b, const Matrix& c);

std::tuple<double, Matrix, Matrix, Matrix, Matrix> simplex(const std::string& problem);

#endif