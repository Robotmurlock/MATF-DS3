#include <iostream>
#include <fstream>
#include <utility>
#include <climits>
#include <cfloat>
#include <cmath>
#include <ctime>
#include "lib/matrix.hpp"

#define STOP ((unsigned)-1)
#define INF DBL_MAX

#define _DEBUG1

// precision: EPS
#define EPS 0.0001

template<typename T>
void vector_print(const std::vector<T>& v)
{
    for(auto e: v)
        std::cout << e << " ";
    std::cout << std::endl;
}

bool is_not_canonical(const Matrix& A, const Matrix& b)
{
    // Checking if b > 0
    for(unsigned i=0; i<b.width(); i++)
        if(b.at(0, i) < 0)
            return true;

    // Checking if b has canonical base
    // 1 0 ...   0 ...
    // 0 1 0 ... 0 ...
    // ...         ...
    // 0 ...     1 ...

    // fabs(A.at(i, j) - 1) > EPS instead of A.at(i, j) != 1 because of precision
    // A.at(i, j) might be 0.99999999

    for(unsigned i=0; i<A.height(); i++)
        for(unsigned j=0; j<A.height(); j++)
            if(i == j && std::fabs(A.at(i, j)-1) > EPS)
                return true;
            else if(i != j && std::fabs(A.at(i, j)) > EPS)
                return true;

    return false;
}

std::tuple<std::vector<unsigned>, std::vector<unsigned>, double> set_canonical_matrix(Matrix& A, Matrix& b, Matrix& c)
{
    srand(time(NULL));

    auto n = A.height();
    auto m = A.width();
    double Fo = 0.0;
    // i ~ column, j ~ row
    while(is_not_canonical(A, b)) 
    {
        std::cout << A << std::endl;
        std::cout << b << std::endl;
        for(unsigned i=0; i<n; i++)
        {
            if(A.at(i, i)*b.at(0, i) < 0)
            {
                std::vector<unsigned> potential_base_columns;
                for(unsigned j=i+1; j<m; j++)
                    if(A.at(i, j)*b.at(0, i) > 0)
                    {
                        potential_base_columns.push_back(j);
                    }

                unsigned new_column_index = rand() % potential_base_columns.size();
                unsigned new_column = potential_base_columns.at(new_column_index);
                swap_columns(A, i, new_column);
                swap_columns(c, i, new_column);
                break;
            }
            // clearing i-th column

            for(unsigned j=0; j<n; j++)
            {
                if(i == j)
                    continue;
                double coef = A.at(j, i)/A.at(i, i);
                for(unsigned k=0; k<m; k++)
                    A.at(j, k) -= coef*A.at(i, k);
                b.at(0, j) -= coef*b.at(0, i);
            }
            double coef = A.at(i, i);
            for(unsigned k=0; k<m; k++)
                A.at(i, k) /= coef;
            b.at(0, i) /= coef;

            coef = c.at(0, i)/A.at(i, i);
            for(unsigned k=0; k<m; k++)
                c.at(0, k) -= coef*A.at(i, k);
            Fo -= coef*b.at(0, i);
        }
    }

    std::vector<unsigned> P, Q;
    for(unsigned i=0; i<m; i++)
        if(i<n)
            P.push_back(i);
        else
            Q.push_back(i);
    return std::make_tuple(P, Q, Fo);
}

Matrix get_B(const Matrix& A, const std::vector<unsigned>& P)
{
    Matrix B = Matrix(A.height(), 0);
    for(auto p: P)
        append(B, A.col(p));

    return B;
}

Matrix get_Cb(const Matrix& c, const std::vector<unsigned>& P)
{
    Matrix Cb = Matrix(1, P.size());
    for(unsigned i=0; i<P.size(); i++)
        Cb.at(0, i) = c.at(0, P.at(i));

    return Cb;
}

Matrix get_Kq(const Matrix& A, const std::vector<unsigned>& Q)
{
    Matrix Kq = Matrix(A.height(), 0);
    for(auto q: Q)
        append(Kq, A.col(q));

    return Kq;
}

Matrix get_Cq(const Matrix& c, const std::vector<unsigned>& Q)
{
    Matrix Cq = Matrix(1, Q.size());
    for(unsigned i=0; i<Q.size(); i++)
        Cq.at(0, i) = c.at(0, Q.at(i));

    return Cq;
}

unsigned get_first_negative(const Matrix& r)
{
    for(unsigned i=0; i<r.width(); i++)
        if(r.at(0, i) < 0)
            return i;
    return STOP;
}

// Starting with x: if c(i) != 0 then we set next unused value of b for x(i) else x(i) = 0
Matrix get_x(const Matrix& c, const Matrix& b)
{
    unsigned j = 0;
    Matrix x = c;
    for(unsigned i=0; i<c.width(); i++)
        if(c.at(0, i) == 0)
            x.at(0, i) = b.at(0, j++);
        else
            x.at(0, i) = 0;

    return x;
}

std::pair<double, unsigned> get_t_opt(const Matrix& x, const Matrix& y, const std::vector<unsigned>& P)
{
    double t = INF;
    unsigned t_index;
    for(unsigned i=0; i<y.width(); i++)
    {
        double val = x.at(0, P.at(i))/y.at(0, i);
        if(y.at(0, i) > 0 && val < t)
        {
            t = val;
            t_index = P.at(i);
        }
    }

    return std::make_pair(t, t_index);
}

void update_x(Matrix& x, const Matrix& y, unsigned l, const std::vector<unsigned>& P, double t_opt)
{
    for(unsigned i=0; i<y.width(); i++)
    {
        unsigned index = P.at(i);
        x.at(0, index) -= y.at(0, i)*t_opt;
    }
    x.at(0, l) = t_opt;
}

void update_P_Q(std::vector<unsigned>& P, std::vector<unsigned>& Q, unsigned t_index, unsigned l)
{
    for(auto& p: P)
        if(p == t_index)
        {
            p = l;
            break;
        }
    for(auto& q: Q)
        if(q == l)
        {
            q = t_index;
            break;
        }
}

bool has_all_negative(const Matrix& y)
{
    for(unsigned i=0; i<y.width(); i++)
        if(y.at(0, i) > 0)
            return false;
    return true;
}

void set_eta(Matrix& E, const Matrix& y, unsigned t_index, const std::vector<unsigned>& P)
{
    unsigned index;
    for(unsigned i=0; i<P.size(); i++)
        if(P.at(i) == t_index)
        {
            index = i;
            break;
        }
    for(unsigned i=0; i<E.height(); i++)
        E.at(i, index) = y.at(0, i);
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
    #ifdef _DEBUG
        std::cout << "Input:" << std::endl;
        std::cout << c << std::endl;
        std::cout << A << std::endl;
        std::cout << b << std::endl << std::endl;
    #endif

    // Rezidual Simplex:
    auto[P, Q, Fo] = set_canonical_matrix(A, b, c);


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

        std::cout << "P&Q:" << std::endl;
        vector_print(P);
        vector_print(Q);
        std::cout << "Fo: " << Fo << std::endl;
        std::cout << std::endl;
    #endif

    // Algorithm starts here ...
    // Preprocess: Calculating x:
    auto x = get_x(c, b);
    auto B = identity(P.size());
    auto E = identity(P.size());
    while(true)
    {
        // Step1: Solve u*B = Cb <=> u = Cb*B' (B' is inverse matrix of B)
        // This is equivalent to u*K(i) = c(i) for i in P which is what we need to find optimal value
        B = B * E;
        auto Cb = get_Cb(c, P);
        auto u = Cb*B.inv();

        // Step2: Calculating r
        // r(j) = c(j) - u*K(j)
        // if (r >= 0) then we found our optimal value
        // This is equivalent to (l_index == STOP) which we get from get_first_negative(r)   
        auto Kq = get_Kq(A, Q);
        auto Cq = get_Cq(c, Q);
        auto r = Cq - u*Kq;

        #ifdef _DEBUG
            std::cout << "x: "  << std::endl << x << std::endl;
            std::cout << "B: "  << std::endl << B << std::endl;
            std::cout << "Cb: " << std::endl << Cb << std::endl;
            std::cout << "u: "  << std::endl << u << std::endl;
            std::cout << "Kq: " << std::endl << Kq << std::endl;
            std::cout << "Cq: " << std::endl << Cq << std::endl;
            std::cout << "r: "  << std::endl << r << std::endl;
        #endif

        auto l_index = get_first_negative(r);
        if(l_index == STOP)
            break;
        auto l = Q.at(l_index);

        // Step3: Solve B*y = Kl <=> y = B'Kl <=> y = B/Kl where r(l) < 0
        auto Kl = A.col(l);
        auto y = (B/Kl).transpose();

        // Step4: If y has all negative values, then there is no optimum value (its not bounded)
        // Otherwise we get t_opt := min{x(i)/y(i) | y(i) > 0}
        if(has_all_negative(y))
        {
            std::cout << "Function does not reach optimal value!" << std::endl;
            return 0;
        }
        auto[t_opt, t_index] = get_t_opt(x, y, P);

        //ETA MATRIX:
        E = identity(B.height());
        set_eta(E, y, t_index, P);
        std::cout << "ETA:" << std::endl;
        std::cout << E << std::endl;

        // Step5: With t_opt we can update our x:
        // x(i) = x_old(i) - t_opt*y(i), for i in P
        // x(i) = t_opt, for i == l
        // x(i) = 0, otherwise
        // We replace t_index in P with l and l in Q with t_index (new base P)
        update_x(x, y, l, P, t_opt);
        update_P_Q(P, Q, t_index, l);

        #ifdef _DEBUG
            std::cout << "l: " << l_index << std::endl;
            std::cout << "Kl:" << std::endl << Kl << std::endl;
            std::cout << "y: " << std::endl << y << std::endl;
            std::cout << "t_opt: " << t_opt << std::endl;
            std::cout << "t_index: " << t_index << std::endl;
            std::cout << "new_x: " << x << std::endl;
            std::cout << "new_P: ";
            vector_print(P);
            std::cout << "new_Q: ";
            vector_print(Q);
        #endif
    }

    std::cout << "Solution: " << x;
    std::cout << "Optimal value: " << -Fo + c*(x.transpose());

    return 0;
}