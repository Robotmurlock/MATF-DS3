#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

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

    std::vector<std::vector<double> > A(m, std::vector<double> (n));
    std::vector<double> b(m);

    for(unsigned i=0; i<m; i++)
        for(unsigned j=0; j<n; j++)
            input >> A.at(i).at(j);

    for(unsigned i=0; i<m; i++)
        input >> b.at(i);

    std::vector<double> X(n);
    for(unsigned i=0; i<n; i++)
        input >> X.at(i);

    for(unsigned i=0; i<m; i++)
    {
        for(unsigned j=0; j<n; j++)
            b.at(i) -= A.at(i).at(j)*X.at(j);
        if(b.at(i) > 0)
        {
            std::cout << "Given point is not a solution!" << std::endl;
            return 0;
        }
    }

    std::cout << "Given point is a solution!" << std::endl;

    return 0;
}