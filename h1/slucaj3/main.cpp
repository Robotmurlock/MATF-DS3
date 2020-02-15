#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

#define INF std::numeric_limits<double>::infinity()

#define _DEBUG1

template<typename T>
void print_vector(const std::vector<T>& v)
{
    for(unsigned i=0; i<v.size(); i++)
        std::cout << v.at(i) << " ";
    std::cout << std::endl;
}

void print_matrix(const std::vector<std::vector<double> >& M)
{
    if(M.size() == 0)
        return;
    auto m = M.size();
    auto n = M.at(0).size();
    for(unsigned i=0; i<m; i++)
    {
        for(unsigned j=0; j<n; j++)
            std::cout << M.at(i).at(j) << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::pair<double, double> interval_intersection(const std::vector<std::vector<double> >& A, const std::vector<double>& b)
{
    auto m = b.size();
    double left = -INF;
    double right = INF;

    for(unsigned p=0; p<m; p++)
    {
        auto value = b.at(p)/A.at(p).at(0);
        if(A.at(p).at(0) < 0)
            right = std::min(right, value);
        else
            left = std::max(left, value);
        
    }

    if(right < left)
        std::cout << "There is no solution!" << std::endl;
    else
    {
        std::cout << "VAR is in ";
        if(left == -INF)
            std::cout << "(-inf, ";
        else
            std::cout << "[" << left << ", ";

        if(right == INF)
            std::cout << "inf)";
        else
            std::cout << right << "]";

        std::cout << "!" << std::endl;
    } 

    return std::make_pair(left, right);
}

std::pair<double, double> eliminate(const std::vector<std::vector<double> >& A, const std::vector<double>& b)
{
    auto m = A.size();
    auto n = A.at(0).size();

    unsigned elim_index = n-1;

    std::vector<unsigned> I, J, K;
    for(unsigned i=0; i<m; i++)
    {
        if(A.at(i).at(elim_index) > 0)
            I.push_back(i);
        else if(A.at(i).at(elim_index) < 0)
            J.push_back(i);
        else 
            K.push_back(i);
    }

    #ifdef _DEBUG
        std::cout << "ELIMINATION: " << std::endl;
        std::cout << "I: "; print_vector(I);
        std::cout << "J: "; print_vector(J);
        std::cout << "K: "; print_vector(K);
    #endif

    std::vector<std::vector<double> > new_A;
    std::vector<double> new_b;

    for(unsigned i=0; i<I.size(); i++)
        for(unsigned j=0; j<J.size(); j++)
        {
            new_A.push_back(std::vector<double>{});
            for(unsigned p=0; p<n; p++)
            {
                if(p == elim_index)
                    continue;
                new_A.back().push_back(A.at(I.at(i)).at(p)/A.at(I.at(i)).at(elim_index) - A.at(J.at(j)).at(p)/A.at(J.at(j)).at(elim_index));
            }
            new_b.push_back(b.at(I.at(i))/A.at(I.at(i)).at(elim_index) - b.at(J.at(j))/A.at(J.at(j)).at(elim_index));
        }

    for(unsigned k=0; k<K.size(); k++)
    {
        new_A.push_back(std::vector<double>{});
        for(unsigned p=0; p<n; p++)
        {
            if(p == elim_index)
                continue;
            new_A.back().push_back(A.at(K.at(k)).at(p));
        }
        new_b.push_back(b.at(K.at(k)));
    }
    #ifdef _DEBUG
        auto new_m = new_b.size();
        std::cout << "new_m: " << new_m << std::endl;
        print_matrix(new_A);
        print_vector(new_b);
    #endif

    // new_n = n-1 --> eliminate if new_n > 1 <=> n > 2
    if(n > 2)
        return eliminate(new_A, new_b);
    else
    {
        return interval_intersection(new_A, new_b);
    }
}

void transform(std::vector<std::vector<double> >& A, std::vector<double>& b, unsigned index, double value)
{
    auto m = A.size();
    for(unsigned p=0; p<m; p++)
    {
        b.at(p) -= A.at(p).at(index)*value;
        A.at(p).erase(std::begin(A.at(p)) + index);
    }

    #ifdef _DEBUG
        print_matrix(A);
    #endif
}

void replace_var_with_function(std::vector<std::vector<double> >& A, std::vector<double>& b, std::vector<double>& c)
{
    auto m = b.size();
    auto n = c.size();
    if(n == 0)
        return;
    
    for(unsigned i=0; i<m; i++)
    {
        A.at(i).at(0) *= (1.0/c.at(0));
        for(unsigned j=1; j<n; j++)
            A.at(i).at(j) += (c.at(j)/c.at(0))*A.at(i).at(0);
    }
}

int main()
{
    // *INPUT FILE*

    std::ifstream input("input.txt");
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

    std::vector<double> c(n);
    std::vector<std::vector<double> > A(m, std::vector<double> (n));
    std::vector<double> b(m);

    for(unsigned i=0; i<n; i++)
        input >> c.at(i);

    for(unsigned i=0; i<m; i++)
        for(unsigned j=0; j<n; j++)
        {
            input >> A.at(i).at(j);
            A.at(i).at(j) *= -1;
        }

    for(unsigned i=0; i<m; i++)
    {
        input >> b.at(i);
        b.at(i) *= -1;
    }
    
    for(unsigned i=0; i<n; i++)
    {
        std::vector<double> tmp(n, 0);
        tmp.at(i) = 1;
        A.push_back(tmp);
        b.push_back(0);
    }

    replace_var_with_function(A, b, c);

    #ifdef _DEBUG
        std::cout << "INPUT: " << std::endl;
        print_vector(c);
        std::cout << std::endl;
        print_matrix(A);
        print_vector(b);
        std::cout << std::endl;
    #endif

    auto interval = eliminate(A, b);

    if(interval.first < interval.second)
        std::cout << "Solution: " << interval.first << std::endl;
    else
        std::cout << "There is no solution!" << std::endl;

    return 0;
}