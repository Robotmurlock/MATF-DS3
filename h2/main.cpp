#include <iostream>
#include <fstream>
#include <utility>
#include "lib/matrix.hpp"

#define _DEBUG

std::pair<std::vector<int>, std::vector<int> > set_canonical_matrix(Matrix& A, Matrix& b, Matrix& c)
{
    auto n = A.height();
    auto m = A.width();
    // i ~ column, j ~ row
    for(unsigned i=0; i<n; i++)
    {
        // clearing i-th columnt 
        for(unsigned j=0; j<n; j++)
        {
            if(i == j)
                continue;
            auto coef = A.at(j, i)/A.at(i, i);
            for(unsigned k=0; k<m; k++)
                A.at(j, k) -= coef*A.at(i, k);
            b.at(0, j) -= coef*b.at(0, i);
        }
        auto coef = c.at(0, i)/A.at(i, i);
        for(unsigned k=0; k<m; k++)
            c.at(0, k) -= coef*A.at(i, k);

        coef = A.at(i, i);
        for(unsigned k=0; k<m; k++)
            A.at(i, k) /= coef;
        b.at(0, i) /= coef;
    }

    std::vector<int> P, Q;
    for(unsigned i=0; i<m; i++)
        if(i<n)
            P.push_back(i);
        else
            Q.push_back(i);
    return std::make_pair(P, Q);
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
            for(unsigned j=0; j<m; j++) 
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
    #ifdef _DEBUG
        std::cout << "Input:" << std::endl;
        std::cout << c << std::endl;
        std::cout << A << std::endl;
        std::cout << b << std::endl << std::endl;
    #endif

    // Rezidual Simplex:
    auto[P, Q] = set_canonical_matrix(A, b, c);

    #ifdef _DEBUG
        for(auto p: P)
            std::cout << p << " ";
        std::cout << std::endl;
        for(auto q: Q)
            std::cout << q << " ";
        std::cout << std::endl;

        std::cout << "set_canonical_matrix:" << std::endl;
        std::cout << c << std::endl;
        std::cout << A << std::endl;
        std::cout << b << std::endl << std::endl;
    #endif

    return 0;
}