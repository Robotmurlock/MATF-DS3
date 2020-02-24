#include <vector>
#include <iostream>
#include <functional>
#include <numeric>

class Matrix {
private:
    std::vector<std::vector<double> > m_elements;
    unsigned m_width, m_height;

    void init(unsigned height, unsigned width, double value);
    std::vector<double> to_vector() const;

public:
    // init: A(height, width, value)
    // init: A(0, 0, 0)
    Matrix();
    // init: A(size, size, 0)
    Matrix(unsigned size);
    // init: A(height, width, 0)
    Matrix(unsigned height, unsigned width);
    // init: A(heigth, width, value)
    Matrix(unsigned height, unsigned width, double value);
    // init: A = elements
    Matrix(std::vector<std::vector<double> > elements);
    Matrix(std::vector<double> vec);

    unsigned height() const;
    unsigned width() const;

    Matrix(const Matrix& M);
    Matrix operator=(const Matrix& M);
    ~Matrix() = default;

    // indexing
    double at(unsigned i, unsigned j) const;
    double& at(unsigned i, unsigned j);

    friend std::ostream& operator<<(std::ostream& out, const Matrix& M);

    Matrix row(unsigned i) const;
    Matrix col(unsigned i) const;
    Matrix transpose() const;

    Matrix operator*(double scalar) const;
    friend Matrix operator*(double scalar, const Matrix& M);
    Matrix operator+(double scalar) const;
    friend Matrix operator+(double scalar, const Matrix& M);
    Matrix operator*(const Matrix& M) const;
    Matrix product(const Matrix& M) const;
    Matrix operator+(const Matrix& M) const;
    Matrix operator-(const Matrix& M) const;

    Matrix _minor(unsigned r, unsigned c) const;
    double det() const;
    Matrix adj() const;
    Matrix inv() const;

    // solves system: Ax = b <-> x = A/b
    Matrix operator/(const Matrix& b) const;

    // norm p = 1
    double norm1() const;

    bool operator==(const Matrix& M) const;
    bool operator!=(const Matrix& M) const;

    friend void append(Matrix& A, const Matrix& B);
    friend void swap_columns(Matrix& A, unsigned i, unsigned j);
};

Matrix identity(unsigned size);