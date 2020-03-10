#include <iostream>
#include <fstream>
#include <utility>
#include <climits>
#include <cfloat>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <vector>
#include "../../lib/matrix.hpp"

#define STOP ((unsigned)-1)
#define INF DBL_MAX

#define BAR "------------------------------------------------------------------------------------------------------------"

// precision: EPS
#define EPS 0.0001

template<typename T>
void vector_print(const std::vector<T>& v)
{
    for(auto e: v)
        std::cout << e << " ";
    std::cout << std::endl;
}

void show_system(const Matrix& A, const Matrix& b, const Matrix& c, double F)
{
    unsigned n = A.width();
    unsigned m = A.height();
    unsigned dim = 10;
        unsigned dashes_cnt = (dim+3)*n+dim;
    std::string dashes(dashes_cnt, '-');

    for(unsigned i=0; i<n; i++)
    {
        std::cout.width(dim-1);
        std::cout << "x" << i << " | ";
    }
    std::cout.width(dim);
    std::cout << "b" << std::endl;

    std::cout << dashes << std::endl;
    for(unsigned i=0; i<m; i++)
    {
        for(unsigned j=0; j<n-1; j++)
        {
            std::cout.width(dim);
            std::cout << A.at(i, j) << "   ";
        }
        std::cout.width(dim);
        std::cout << A.at(i, n-1) << " | ";
        std::cout.width(dim);
        std::cout << b.at(0, i) << std::endl;
    }
    std::cout << dashes << std::endl;
    for(unsigned j=0; j<n-1; j++)
    {
        std::cout.width(dim);
        std::cout << c.at(0, j) << "   ";
    }
    std::cout.width(dim);
    std::cout << c.at(0, n-1) << " | ";
    std::cout.width(dim);
    std::cout << F << std::endl;
    std::cout << std::endl;

}

unsigned is_non_negative(const Matrix& b)
{
    for(unsigned i=0; i<b.width(); i++)
        if(b.at(0, i) < 0)
            return i;
    return STOP;
}

unsigned is_non_negative_row(const Matrix& A, unsigned row)
{
    for(unsigned i=0; i<A.width(); i++)
        if(A.at(row, i) < 0)
            return i;
    return STOP;
}

unsigned find_pivot(const Matrix& A, const Matrix& c, unsigned row)
{
    unsigned pivot_index;
    double max_value = -INF;
    for(unsigned i=0; i<A.width(); i++)
        if(A.at(row, i) < 0)
        {
            double value = c.at(0, i)/A.at(row, i);
            if(value > max_value)
            {
                max_value = value;
                pivot_index = i;
            }
        }
    return pivot_index;
}

void update_system(Matrix& A, Matrix& b, Matrix& c, double& F, unsigned p_row, unsigned p_col)
{
    unsigned n = A.width();
    unsigned m = A.height();
    // divide row of pivot by its value
    double coef = A.at(p_row, p_col);
    for(unsigned i=0; i<n; i++)
        A.at(p_row, i) /= coef;
    b.at(0, p_row) /= coef;
    // "clear" p_col columns
    for(unsigned i=0; i<m; i++)
    {
        if(i == p_row)
            continue;
        coef = -A.at(i, p_col);
        for(unsigned j=0; j<n; j++)
            A.at(i, j) += coef*A.at(p_row, j);
        b.at(0, i) += coef*b.at(0, p_row);
    }
    coef = -c.at(0, p_col);
    for(unsigned j=0; j<n; j++)
            c.at(0, j) += coef*A.at(p_row, j);
    F += coef*b.at(0, p_row);
}

int main(int argc, char** argv)
{
    std::cout << std::fixed;
    std::cout << std::setprecision(2);

    // *INPUT FILE*
    const char* path = (argc >= 2) ? argv[1] : "input.txt";

    std::ifstream input(path);
    if(input.fail())
    {
        std::cout << "Failed to open \"input.txt\"!" << std::endl;
        return 1;
    }

    // *TASK INPUT*

    // n - number of vars
    // m - number of inequalities
    unsigned n, m;
    input >> n >> m;

    std::vector<double> in_c(n);
    std::vector<std::vector<double> > in_A(m, std::vector<double> (n));
    std::vector<double> in_b(m);
    std::string eq_sign;

    for(unsigned i=0; i<n; i++)
        input >> in_c.at(i);

    unsigned new_n = n;
    for(unsigned i=0; i<m; i++)
    {
        for(unsigned j=0; j<n; j++)
            input >> in_A.at(i).at(j);
        input >> eq_sign;
        input >> in_b.at(i);

        if(eq_sign == "=")
            continue;
        
        // if sign is > then transform it to <
        if(eq_sign == ">")
        {
            for(unsigned j=0; j<n; j++) 
                in_A.at(i).at(j) *= -1;
            in_b.at(i) *= -1;
        }

        // add new var to convert inequation to equation:
        // x1 + x2 + x3 ... xN <= c
        // => x1 + x2 + x3 ... xN + x = c
        new_n++;
        for(unsigned j=0; j<m; j++)
            in_A.at(j).push_back((i==j) ? 1 : 0);
        in_c.push_back(0);
    }

    Matrix c(in_c), A(in_A), b(in_b);
    double F = 0;
    show_system(A, b, c, F);

    unsigned iteration = 0;
    while(true)
    {
        std::cout << "ITERATION: " << iteration++ << std::endl;
        // STEP1: Is b >= 0?
        // If YES then: STOP, optimal value is found
        // If NO  then: exists i such that b(i) < 0 => STEP2
        std::cout << "STEP1: Is b >= 0?" << std::endl;
        unsigned b_negative_index;
        if((b_negative_index = is_non_negative(b)) == STOP)
        {
            std::cout << "YES! optimal value is found!" << std::endl;
            std::cout << std::endl;
            std::cout << "Optimal value: " << -F << std::endl;
            return 0;
        } 
        else
        {
            std::cout << "NO! b(" << b_negative_index << ") is negative." << std::endl;
            std::cout << std::endl;
            // STEP2: Is A(b_negative_index) >= 0?
            // If YES then: STOP, there is no solution
            // If NO  then: exists j such that A(b_negative_index, j) < 0
            std::cout << "STEP2: Is A(" << b_negative_index <<") >= 0?" << std::endl;
            unsigned A_negative_index;
            if((A_negative_index = is_non_negative_row(A, b_negative_index)) == STOP)
            {
                std::cout << "YES! There is no solution!" << std::endl;
                return 0;
            }
            else
            {
                std::cout << "NO! A(" << b_negative_index << ", " << A_negative_index << ") is negative." << std::endl;
                std::cout << std::endl;

                // STEP3: Finding max c(j)/A(b_negative_index, j) such that A(b_negative_index, j) < 0
                std::cout << "STEP3: Finding max c(j)/A(" << b_negative_index 
                        << ", j) such that A(" << b_negative_index << ", j) < 0" << std::endl;
                unsigned pivot_index = find_pivot(A, c, b_negative_index);
                std::cout << "Pivot: A(" << b_negative_index << ", " <<  pivot_index << ")" << std::endl;
                std::cout << std::endl;
                
                update_system(A, b, c, F, b_negative_index, pivot_index);
                show_system(A, b, c, F);
            }
        }
    }

    return 0;
}