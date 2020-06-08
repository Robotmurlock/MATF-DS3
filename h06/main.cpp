#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "../lib/matrix.hpp"

#define NUMSPACING 6

void print_matrix(const std::vector<std::vector<double> >& mat)
{
    std::cout.fill(' ');
    for(unsigned i=0; i<mat.size(); i++)
    {
        for(unsigned j=0; j<mat.at(i).size(); j++)
        {
            std::cout.width(NUMSPACING);
            std::cout << mat.at(i).at(j) << " ";
        }
        std::cout << std::endl; 
    }
    std::cout << std::endl;
}

void domination(std::vector<std::vector<double> >& game)
{
    unsigned n = game.size();
    unsigned m = game.at(0).size();

    std::vector<bool> is_removed_row(n, false);
    std::vector<bool> is_removed_col(m, false);

    bool has_changes = true;
    while(has_changes)
    {
        has_changes = false;
        // Finding dominated rows
        for(unsigned i=0; i<n; i++)
        {
            if(is_removed_row.at(i))
                continue;

            for(unsigned j=i+1; j<n; j++)
            {
                if(is_removed_row.at(j))
                    continue;

                bool is_dominated_i = true, is_dominated_j = true;
                for(unsigned k=0; k<m; k++)
                {
                    if(is_removed_col.at(k))
                        continue;

                    if(game.at(i).at(k) > game.at(j).at(k))
                        is_dominated_i = false;
                    if(game.at(i).at(k) < game.at(j).at(k))
                        is_dominated_j = false;

                    // minor optimization
                    if(!is_dominated_i && !is_dominated_j)
                        continue;
                }

                if(is_dominated_j)
                {
                    is_removed_row.at(j) = true;
                    has_changes = true;
                }
                else if(is_dominated_i)
                {
                    is_removed_row.at(i) = true;
                    has_changes = true;
                }
            }
            if(has_changes)
                continue;
        }
        if(has_changes)
            continue;

        // Finding dominated columns
        for(unsigned i=0; i<m; i++)
        {
            if(is_removed_col.at(i))
                continue;

            for(unsigned j=i+1; j<m; j++)
            {
                if(is_removed_col.at(j))
                    continue;

                bool is_dominated_i = true, is_dominated_j = true;
                for(unsigned k=0; k<n; k++)
                {
                    if(is_removed_row.at(k))
                        continue;

                    if(game.at(k).at(i) < game.at(k).at(j))
                        is_dominated_i = false;
                    if(game.at(k).at(i) > game.at(k).at(j))
                        is_dominated_j = false;

                    // minor optimization
                    if(!is_dominated_i && !is_dominated_j)
                        continue;
                }

                if(is_dominated_j)
                {
                    is_removed_col.at(j) = true;
                    has_changes = true;
                }
                else if(is_dominated_i)
                {
                    is_removed_col.at(i) = true;
                    has_changes = true;
                }
            }
            if(has_changes)
                continue;
        }
        if(has_changes)
            continue;
    }

    std::vector<std::vector<double> > new_game;
    for(unsigned i=0; i<n; i++)
    {
        if(is_removed_row.at(i))
            continue;
        std::vector<double> new_row;
        for(unsigned j=0; j<m; j++)
            if(!is_removed_col.at(j))
                new_row.push_back(game.at(i).at(j));
        new_game.push_back(new_row);
    }

    game = new_game;
}

void print_problems(const std::vector<std::vector<double> >& game)
{
    unsigned n = game.size();
    unsigned m = game.at(0).size();

    std::cout << "First problem:" << std::endl; 
    std::cout << "(min) -v" << std::endl;

    for(unsigned i=0; i<n; i++)
    {
        std::cout << "\t";
        for(unsigned j=0; j<m; j++)
            std::cout << game.at(i).at(j) << "*x" << j+1 << " ";
        std::cout << ">= v" << std::endl;
    }
    std::cout << "\t";
    for(unsigned i=0; i<n; i++)
    {
        std::cout << "x" << i+1 << " ";
        if(i != n-1)
            std::cout << "+ ";
    }
    std::cout << "= 1" << std::endl;
    std::cout << "\t";
    for(unsigned i=0; i<n; i++)
    {
        std::cout << "x" << i+1;
        if(i != n-1)
            std::cout << ", ";
    }
    std::cout << " >= 0" << std::endl;
    std::cout << std::endl;

    std::cout << "Second problem:" << std::endl; 
    std::cout << "(min) v" << std::endl;

    for(unsigned i=0; i<m; i++)
    {
        std::cout << "\t";
        for(unsigned j=0; j<n; j++)
            std::cout << game.at(j).at(i) << "*y" << j+1 << " ";
        std::cout << "<= v" << std::endl;
    }
    std::cout << "\t";
    for(unsigned i=0; i<m; i++)
    {
        std::cout << "y" << i+1 << " ";
        if(i != n-1)
            std::cout << "+ ";
    }
    std::cout << "= 1" << std::endl;
    std::cout << "\t";
    for(unsigned i=0; i<m; i++)
    {
        std::cout << "y" << i+1;
        if(i != n-1)
            std::cout << ", ";
    }
    std::cout << " >= 0" << std::endl;
    std::cout << std::endl;

}

void form_problems(const std::vector<std::vector<double> >& game)
{
    unsigned n = game.size();
    unsigned m = game.at(0).size();

    std::ofstream output1("problem1.txt");
    std::ofstream output2("problem2.txt");

    output1 << n+1 << " " << m+2 << std::endl << std::endl;
    output2 << m+1 << " " << n+2 << std::endl << std::endl;

    output1 << "-1 ";
    for(unsigned i=0; i<m; i++)
        output1 << "0 ";
    output1 << std::endl << std::endl;
    
    output2 << "1 ";
    for(unsigned i=0; i<n; i++)
        output2 << "0 ";
    output2 << std::endl << std::endl;
    

    for(unsigned i=0; i<n; i++)
    {
        output1 << "1 ";
        for(unsigned j=0; j<m; j++)
            output1 << game.at(i).at(j)*(-1) << " ";
        output1 << std::endl;
        
    }
    for(unsigned i=0; i<m; i++)
    {
        output2 << "-1 ";
        for(unsigned j=0; j<n; j++)
            output2 << game.at(j).at(i) << " ";
        output2 << std::endl;
    }

    output1 << "0 ";
    for(unsigned i=0; i<n; i++)
        output1 << "1 ";
    output1 << std::endl;
    output1 << "0 ";
    for(unsigned i=0; i<n; i++)
        output1 << "-1 ";
    output1 << std::endl;
    output2 << "0 ";
    for(unsigned i=0; i<m; i++)
        output2 << "1 ";
    output2 << std::endl;
    output2 << "0 ";
    for(unsigned i=0; i<m; i++)
        output2 << "-1 ";
    output2 << std::endl;

    output1 << std::endl;
    for(unsigned i=0; i<n; i++)
        output1 << "0 ";
    output1 << "1 -1" << std::endl;
    output2 << std::endl;
    for(unsigned i=0; i<m; i++)
        output2 << "0 ";
    output2 << "1 -1" << std::endl;

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

    std::vector<std::vector<double> > game(n, std::vector<double>(m));

    for(unsigned i=0; i<n; i++)
        for(unsigned j=0; j<m; j++)
            input >> game.at(i).at(j);

    std::cout << "Initial game matrix: " << std::endl;
    print_matrix(game);
    domination(game);
    std::cout << "Game matrix after removing dominated rows and columns: " << std::endl;
    print_matrix(game);

    print_problems(game);
    form_problems(game);

    return 0;
}