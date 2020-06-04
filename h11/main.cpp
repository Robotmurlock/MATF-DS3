#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

const unsigned number_space = 5;

void error(std::string msg)
{
    std::cout << "Error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

// Ispis problem u obliku tabele
void problem_info(unsigned backpack_space, const std::vector<double>& prices, const std::vector<unsigned>& weights)
{
    std::cout << std::setfill(' ');
    std::cout << "backpack space: " << backpack_space << std::endl;
    std::cout << "item   │";
    for(unsigned i=0; i<prices.size(); i++)
    {
        std::cout << std::setw(number_space);
        std::cout << ("x" + std::to_string(i+1)) << "|";
    }
    std::cout << std::endl;
    std::cout << "───────□";
    for(unsigned i=0; i<prices.size(); i++)
    {
        for(unsigned j=0; j<number_space; j++)
            std::cout << "─";
        std::cout << "□";
    }
    std::cout << std::endl;
    std::cout << "price  |";
    for(unsigned i=0; i<prices.size(); i++)
    {
        std::cout << std::setw(number_space);
        std::cout << prices.at(i) << "|";
    }
    std::cout << std::endl;
    std::cout << "weight |";
    for(unsigned i=0; i<weights.size(); i++)
    {
        std::cout << std::setw(number_space);
        std::cout << weights.at(i) << "|";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void solve(unsigned backpack_space, const std::vector<double>& prices, const std::vector<unsigned>& weights)
{
    unsigned n = prices.size();
    std::vector<std::vector<double> > F(n+1, std::vector<double>(backpack_space+1, 0));
    std::vector<std::vector<unsigned> > index(n+1, std::vector<unsigned>(backpack_space+1, 0));
    // bs ~ "trenutna dimenzija ranca"
    // Inicijalizacija svih F1(bs) tj. prvog nivoa
    for(unsigned bs=1; bs<=backpack_space; bs++)
    {
        F.at(1).at(bs) = prices.at(0) * (bs/weights.at(0));
        index.at(1).at(bs) = bs/weights.at(0);
    }

    // Popunjavanje F tabele
    // k ~ indeks promenljive
    // bs ~ dimenzija ranca
    // Resava se problem Fk(bs)
    for(unsigned k=2; k<=n; k++)
    {
        for(unsigned bs=1; bs<=backpack_space; bs++)
        {
            // Koristi se prva rekurentna formula za izracunavanje optimalnog Fk(bs)
            // koristi se prices.at(k-1) umesto prices.at(k) jer x1 odgovara indeksu 0
            // slicno i za weghts
            // Fk(bs) = max{prices.at(k-1)*cnt + Fk-1(bs-cnt*weight.at(k-1)) | cnt in {0, ... , bs/weights.at(k-1)}}
            // Iterativno se izracunavava maksimum svih ovih vrednosti
            double optimal_F_bs_k = 0.0;
            unsigned optimal_cnt;
            // space == bs-cnt*weight.at(k-1)
            int space = bs;
            unsigned cnt = 0;
            while(space > 0)
            {
                double value = cnt*prices.at(k-1) + F.at(k-1).at(space);
                if(value > optimal_F_bs_k)
                {
                    optimal_F_bs_k = value;
                    optimal_cnt = cnt;
                }
                
                space -= weights.at(k-1);
                cnt++;
            }
            // Beleze se optimalna vrednost i optimalan indeks u tabelu
            F.at(k).at(bs) = optimal_F_bs_k;
            index.at(k).at(bs) = optimal_cnt;
        }
    } 

    // Ispis F-tabele
    std::cout << "F table" << std::endl;
    for(unsigned i=0; i<=n; i++)
    {
        for(unsigned j=0; j<=backpack_space; j++)
        {
            std::cout << std::setw(number_space);
            std::cout << F.at(i).at(j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Ispis indeks tabele
    std::cout << "Index table" << std::endl;
    for(unsigned i=0; i<=n; i++)
    {
        for(unsigned j=0; j<=backpack_space; j++)
        {
            std::cout << std::setw(number_space);
            std::cout << index.at(i).at(j) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Optimal solution: " << F.at(n).at(backpack_space) << std::endl;

    // Rekonstrukcija resenja:
    unsigned k=n;
    unsigned space = backpack_space;
    while(k > 0)
    {
        unsigned cnt = index.at(k).at(space);
        std::cout << (std::string("x" + std::to_string(k))) << " = " << cnt << std::endl;
        space -= cnt*weights.at(k-1);
        k--;
    }
}

int main(int argc, char** argv)
{
    if(argc < 2)
        error(std::string("Wrong number of arguments!"));

    std::string file_name = std::string(argv[1]);
    // ispis brojeva u pokretnem zarezu sa dve decimale
    std::cout << std::setprecision(2);

    std::ifstream input(file_name);
    if(input.fail())
        error(std::string("Opening file failed"));

    // broj predmeta
    unsigned n;
    input >> n;
    // cena predmeta
    std::vector<double> prices(n); 
    for(unsigned i=0; i<n; i++)
        input >> prices.at(i);
    // tezine predmeta
    std::vector<unsigned> weights(n); 
    for(unsigned i=0; i<n; i++)
        input >> weights.at(i);
    // dimenzija ranca
    unsigned backpack_space;
    input >> backpack_space;

    problem_info(backpack_space, prices, weights);
    solve(backpack_space, prices, weights);

    return 0;
}