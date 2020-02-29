#include <iostream>
#include "matrix.hpp"

int main()
{
    Matrix A(3);
    Matrix B(2, 3, 5);
    Matrix C(std::vector<std::vector<double> >{{1,2,3},{2,3,4}});

    std::cout << "Testing output:" << std::endl;
    std::cout << A << std::endl;
    std::cout << B << std::endl;
    std::cout << C << std::endl;

    std::cout << "Testing col and row:" << std::endl;
    std::cout << C.row(1) << std::endl;
    std::cout << C.col(2) << std::endl;

    std::cout << "Testing transpose:" << std::endl;
    std::cout << A.transpose() << std::endl;
    std::cout << C.transpose() << std::endl;

    std::cout << "Multiplication by scalar:" << std::endl;
    std::cout << C*3 << std::endl;
    std::cout << 2*C << std::endl;

    std::cout << "Addition by scalar:" << std::endl;
    std::cout << C+7 << std::endl;
    std::cout << 5+C << std::endl;

    Matrix D(std::vector<std::vector<double> >{{1,2},{3,4},{5,6}});

    std::cout << "Matrix multiplication:" << std::endl;
    std::cout << C*D << std::endl;
    std::cout << B*A << std::endl;
    std::cout << D*B << std::endl;

    std::cout << "Matrix product:" << std::endl;
    std::cout << B.product(C) << std::endl;

    std::cout << "Matrix sum:" << std::endl;
    std::cout << B+C << std::endl;

    std::cout << "Identity matrix:" << std::endl;
    std::cout << identity(3) << std::endl;

    Matrix E(5, 5, 5);

    std::cout << "Matrix minor:" << std::endl;
    std::cout << E._minor(1, 1) << std::endl;
    std::cout << C._minor(0, 0) << std::endl;

    Matrix F(std::vector<std::vector<double> >{{3,0,2},{2,0,-2},{0,1,1}});
    Matrix G(std::vector<std::vector<double> >{{4, 7}, {2, 6}});

    std::cout << "Matrix determinant:" << std::endl;
    std::cout << A.det() << std::endl;
    std::cout << E.det() << std::endl;
    std::cout << F.det() << std::endl;

    std::cout << "Adjugate matrix" << std::endl;
    std::cout << F.adj() << std::endl;

    std::cout << "Matrix inverse" << std::endl;
    std::cout << F.inv() << std::endl;
    std::cout << G.inv() << std::endl;
    
    std::cout << "Matrix inverse test:" << std::endl;
    std::cout << F.inv() * F << std::endl;

    Matrix H(std::vector<std::vector<double> >{{1,1,1},{0,2,5},{2,5,-1}});
    Matrix b(std::vector<std::vector<double> >{{6},{-4},{27}});
    std::cout << "Solving system Ax = b" << std::endl;
    auto x = H/b;
    std::cout << x << std::endl;
    
    std::cout << "Checking solution: " << std::endl;
    std::cout << ((H*x - b).norm1() < 0.000001) << std::endl;

    Matrix copy1 = F;
    Matrix copy2(F);
    std::cout << "Copying Matrix:" << std::endl;
    std::cout << copy1 << std::endl;
    std::cout << copy2 << std::endl;

    std::cout << "Appending matrices:" << std::endl;
    append(A, A);
    std::cout << A << std::endl;
    append(A, D);
    std::cout << A << std::endl;

    std::cout << "Removing column" << std::endl;
    std::cout << C.remove_column(1) << std::endl;
    std::cout << C.remove_row(1) << std::endl;

    return 0;
}