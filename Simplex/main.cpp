#include <iostream>
#include <fstream>
#include <utility>
#include <climits>
#include <cfloat>
#include <cmath>
#include <ctime>
#include <iomanip>
#include "../lib/matrix.hpp"

#define STOP ((unsigned)-1)
#define INF DBL_MAX

#define BAR "------------------------------------------------------"

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

std::tuple<std::vector<unsigned>, std::vector<unsigned>, double> set_unit_matrix(Matrix& A, Matrix& b, Matrix& c)
{
    srand(time(NULL));

    auto n = A.height();
    auto m = A.width();
    double Fo = 0.0;
    // i ~ column, j ~ row
    while(is_not_canonical(A, b)) 
    {
        for(unsigned i=0; i<n; i++)
        {
            if(A.at(i, i)*b.at(0, i) < 0)
            {
                // If b(i) is negative then we have to pick different columns for base
                // Finding all potential base columns:
                std::vector<unsigned> potential_base_columns;
                for(unsigned j=i+1; j<m; j++)
                    if(A.at(i, j)*b.at(0, i) > 0)
                    {
                        potential_base_columns.push_back(j);
                    }

                // Picking random column and replacing it with current one in base:
                unsigned new_column_index = rand() % potential_base_columns.size();
                unsigned new_column = potential_base_columns.at(new_column_index);
                swap_columns(A, i, new_column);
                swap_columns(c, i, new_column);
            }
            // "clearing" i-th column
            // "clearing" - Transformation with result of i-th column having
            // 0s above and below i-th row and 1 for i-th row
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

// Starting with x: if i in P then we set next unused value of b for x(i) else x(i) = 0
Matrix get_x(const Matrix& b, const std::vector<unsigned>& P, unsigned size)
{
    unsigned j = 0;
    Matrix x(1, size, 0);
    for(auto p: P)
        x.at(0, p) = b.at(0, j++);

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

std::pair<double, Matrix> residual_simplex(Matrix& A, Matrix& b, Matrix& c,
                                           std::vector<unsigned>& P, std::vector<unsigned>& Q, double Fo)
{

    // Preprocess: Calculating x:
    auto x = get_x(b, P, c.width());
    std::cout << "Starting x value: " << x << std::endl;
    unsigned iteration = 0;
    while(true)
    {
        std::cout << BAR << std::endl;
        std::cout << "ITERATION " << iteration++ << ":" << std::endl;
        // Cb ~ contains values from c where c(i) is in Cb if i is in P
        // P = [1, 3, 4], C = [c1, c2, ... cN] => Cb = [c1, c3, c4]

        // Step1: Solve u*B = Cb <=> u = Cb*B' (B' is inverse matrix of B)
        // This is equivalent to u*K(i) = c(i) for i in P which is what we need to find optimal value

        auto B = get_B(A, P);
        auto Cb = get_Cb(c, P);
        auto u = Cb*B.inv();
        std::cout << "Step1: Solving system(1): uB = Cb" << std::endl;
        std::cout << "B:" << std::endl;
        std::cout << B << std::endl;
        std::cout << "Cb: " << Cb << std::endl;
        std::cout << "Result of u(1):" << u << std::endl;

        // Step2: Calculating r
        // r(j) = c(j) - u*K(j)
        // if (r >= 0) then we found our optimal value
        // This is equivalent to (l_index == STOP) which we get from get_first_negative(r)   
        auto Kq = get_Kq(A, Q);
        auto Cq = get_Cq(c, Q);
        auto r = Cq - u*Kq;
        // K := Kq in output
        // C := Cq in output
        std::cout << "Step2: Calculating r (r := C - uK):" << std::endl;
        std::cout << "C: " << Cq << std::endl;
        std::cout << "K: " << std::endl;
        std::cout << Kq << std::endl; 
        std::cout << "Result(r): " << r << std::endl;

        // If r > 0 then optimal value is found
        auto l_index = get_first_negative(r);
        if(l_index == STOP)
        {
            std::cout << "(r > 0) is true => optimal value is found!" << std::endl;
            break;
        }
        auto l = Q.at(l_index);
        std::cout << "Bland's rule: first negative r(i) is r" << l_index << "!" << std::endl;

        // Step3: Solve B*y = Kl <=> y = B'Kl <=> y = B/Kl where r(l) < 0
        auto Kl = A.col(l);
        auto y = (B/Kl).transpose();
        std::cout << "Step3: Solving system(2): By = K" << l_index << std::endl;
        std::cout << "B:" << std::endl;
        std::cout << B << std::endl;
        std::cout << "K" << l << ": " << std::endl << Kl << std::endl;
        std::cout << "Result of y(2):" << y << std::endl;

        // Step4: If y has all negative values, then there is no optimum value (its not bounded)
        // Otherwise we get t_opt := min{x(i)/y(i) | y(i) > 0}
        std::cout << "Step4: check if y <= 0:" << std::endl;
        if(has_all_negative(y))
        {
            std::cout << "Function does not reach optimal value because (y <= 0) is true!" << std::endl;
            return std::make_pair(0, Matrix());
        }
        std::cout << "(y <= 0) is not true!" << std::endl;
        std::cout << "Finding optimal t:" << std::endl;
        auto[t_opt, t_index] = get_t_opt(x, y, P);
        std::cout << "Optimal t: " << t_opt << std::endl;
        std::cout << "Column " << t_index << " leaves base (P)" << std::endl;


        // Step5: With t_opt we can update our x:
        // x(i) = x_old(i) - t_opt*y(i), for i in P
        // x(i) = t_opt, for i == l
        // x(i) = 0, otherwise
        // We replace t_index in P with l and l in Q with t_index (new base P)
        std::cout << "Step5: updating x:" << std::endl;
        std::cout << "Old x: " << x;
        update_x(x, y, l, P, t_opt);
        update_P_Q(P, Q, t_index, l);
        std::cout << "New x: " << x << std::endl;
    }
    std::cout << BAR << std::endl;

    // c*(x.transpose()) is matrix with dimension 1x1
    double F = -Fo + (c*(x.transpose())).at(0, 0);
    return std::make_pair(F, x);
}

void show_system(const Matrix& A, const Matrix& b, const Matrix& c)
{
    std::cout << "c: " << c << std::endl;
    std::cout << "A:" << std::endl;
    std::cout << A << std::endl;
    std::cout << "b: " << b << std::endl << std::endl;
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
    std::cout << "Solving system(canonical form): Ax = b" << std::endl;
    show_system(A, b, c);
    std::cout << BAR << std::endl;

    // Rezidual Simplex:
    // P ~ column indexes of base matrix B
    // Q ~ other column indexes
    // Fo ~ base value of F where F = Fo + c*x
    // x ~ solution
    auto[P, Q, Fo] = set_unit_matrix(A, b, c);

    std::cout << "set_unit_matrix:" << std::endl;
    show_system(A, b, c);

    std::cout << "Base indexes(P): ";
    vector_print(P);
    std::cout << "Nonbase indexes(Q): ";
    vector_print(Q);
    
    std::cout << "Base function value(Fo): " << Fo << std::endl;
    std::cout << BAR << std::endl;

    std::cout << "Residual simplex: " << std::endl;
    auto[F, x] = residual_simplex(A, b, c, P, Q, Fo);

    // if x.height() == 0 and x.width() == 0 then there is no solution (special case value)
    if(x.height() == 0 && x.width() == 0)
        return 0;

    std::cout << "Solution: " << x;
    std::cout << "Optimal value: " << F << std::endl;

    return 0;
}