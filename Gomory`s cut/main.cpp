#include <iostream>
#include <vector>
#include <string>
#include "simplex.hpp"

#define UNUSED_VAR(X) ((void)X)

inline double phi(double value)
{
    return value - ((int)value);
}

class SimplexProblemDefinition {
private:
    unsigned m_variables;
    unsigned m_constraints;

    std::vector<std::vector<double> > m_A;
    std::vector<double> m_b;
    std::vector<double> m_c;
    std::vector<char> m_relsigns;

public:
    SimplexProblemDefinition(
        const std::vector<std::vector<double> >& A,
        const std::vector<double>& b,
        const std::vector<double>& c,
        const std::vector<char>& relsigns)
        : m_A(A), m_b(b), m_c(c), m_relsigns(relsigns)
    {
        m_variables = A.at(0).size();
        m_constraints = A.size();
    }

    SimplexProblemDefinition(const Matrix& A, const Matrix& b, const Matrix& c, double F)
        : m_A(A.to_cpp_matrix()), m_b(b.to_cpp_matrix().at(0)), m_c(c.to_cpp_matrix().at(0))
    {
        m_variables   = A.width();
        m_constraints = A.height();
        m_relsigns.resize(m_constraints, '=');

        m_b.push_back(phi(F));
        for(unsigned i=0; i<m_constraints; i++)
            m_A.at(i).push_back(0.0);
        m_c.push_back(0.0);
        m_variables++;
        m_constraints++;
        std::vector<double> new_constraint;
        for(unsigned i=0; i<m_variables-1; i++)
            new_constraint.push_back(-phi(c.at(0, i)));
        new_constraint.push_back(1.0);
        m_A.push_back(new_constraint);
        m_relsigns.push_back('=');
    }

    std::string format() const
    {
        std::string out = "";
        out += std::to_string(m_variables) + " " + std::to_string(m_constraints) + "\n";
        for(unsigned i=0; i<m_variables; i++)
            out += std::to_string(m_c.at(i)) + " ";
        out += "\n";
        for(unsigned i=0; i<m_constraints; i++)
        {
            for(unsigned j=0; j<m_variables; j++)
                out += std::to_string(m_A.at(i).at(j)) + " ";
            out += std::string() + m_relsigns.at(i) + " ";
            out += std::to_string(m_b.at(i)) + "\n";
        }
        return out;

    }

    bool is_invalid() const
    {
        for(unsigned i=0; i<m_variables-1; i++)
            if(std::fabs(m_A.back().at(i)) > EPS)
                return false;
        return true;
    }

};

bool is_x_integer(std::vector<double> x)
{
    for(unsigned i=0; i<x.size(); i++)
        if(phi(x.at(i)) >= EPS)
        {
            std::cout << x.at(i) << " " << phi(x.at(i)) << std::endl;
            return false;
        }
    return true;
}

int main(int argc, char** argv)
{
    UNUSED_VAR(argc);
    const char* path = argv[1];

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

    std::vector<double> c(n);
    std::vector<std::vector<double> > A(m, std::vector<double> (n));
    std::vector<double> b(m);
    std::vector<char> relsigns(m);

    for(unsigned i=0; i<n; i++)
        input >> c.at(i);

    std::string relsign;
    for(unsigned i=0; i<m; i++)
    {
        for(unsigned j=0; j<n; j++)
            input >> A.at(i).at(j);
        input >> relsign;
        relsigns.at(i) = relsign.at(0);
        input >> b.at(i);
    }

    SimplexProblemDefinition initial_problem(A, b, c, relsigns);

    std::cout << initial_problem.format() << std::endl;
    auto[result_F, result_x, result_A, result_b, result_c] = simplex(initial_problem.format());
    std::cout << "Optimal value: " << result_F << std::endl;
    std::cout << "Variable results: " << result_x << std::endl;
    std::cout << "A: \n" << result_A << std::endl;
    std::cout << "b: " << result_b << std::endl;
    std::cout << "c: " << result_c << std::endl;

    while(!is_x_integer(result_x.to_cpp_matrix().at(0)))
    {
        SimplexProblemDefinition problem(result_A, result_b, result_c, result_F);
        std::cout << problem.format() << std::endl;
        if(problem.is_invalid())
        {
            std::cout << "There is not solution!" << std::endl;
            return 0;
        }
        
        auto result = simplex(problem.format());
        result_F = std::get<0>(result);
        result_x = std::get<1>(result);
        result_A = std::get<2>(result);
        result_b = std::get<3>(result);
        result_c = std::get<4>(result);
    }

    return 0;
}