#include "matrix.hpp"

void Matrix::init(unsigned height, unsigned width, double value)
{
    m_height = height;
    m_width = width;
    m_elements.resize(m_height, std::vector<double> (m_width, value));
}

std::vector<double> Matrix::to_vector() const
{
    std::vector<double> vec(m_width);
    for(unsigned i=0; i<m_width; i++)
        vec.at(i) = m_elements[0][i];
    return vec;
}

Matrix::Matrix()
{
    m_height = 0;
    m_width = 0;
}

Matrix::Matrix(unsigned size)
{
    init(size, size, 0.0);
}

Matrix::Matrix(unsigned height, unsigned width)
{
    init(height, width, 0.0);
}

Matrix::Matrix(unsigned height, unsigned width, double value)
{
    init(height, width, value);
}

Matrix::Matrix(std::vector<std::vector<double> > elements)
    : m_elements(elements)
{
    m_height = elements.size();
    m_width = (m_height > 0) ? elements.at(0).size() : 0;
}

Matrix::Matrix(std::vector<double> vec)
{
    init(1, vec.size(), 0.0);
    for(unsigned i=0; i<vec.size(); i++)
        this->at(0, i) = vec.at(i);
}

Matrix::Matrix(const Matrix& M)
    : m_elements(M.m_elements)
{
    m_height = M.m_height;
    m_width = M.m_width;
}

unsigned Matrix::height() const
{
    return m_height;
}

unsigned Matrix::width() const
{
    return m_width;
}

Matrix Matrix::operator=(const Matrix& M)
{
    this->m_elements = M.m_elements;
    m_height = M.m_height;
    m_width = M.m_width;
    return *this;
}

double Matrix::at(unsigned i, unsigned j) const
{
    return m_elements.at(i).at(j);
}

double& Matrix::at(unsigned i, unsigned j)
{
    return m_elements.at(i).at(j);
}

std::ostream& operator<<(std::ostream& out, const Matrix& M)
{
    for(unsigned i=0; i<M.m_height; i++)
    {
        for(unsigned j=0; j<M.m_width; j++)
        {
            out << M.at(i, j) << " ";
        }
        out << std::endl;
    }

    return out;
}

Matrix Matrix::row(unsigned i) const
{
    std::vector<std::vector<double> > elements(1, m_elements.at(i));
    return Matrix(elements);
}

Matrix Matrix::col(unsigned i) const
{
    std::vector<std::vector<double> > elements(m_height, std::vector<double>(1));
    for(unsigned j=0; j<m_height; j++)
        elements.at(j).at(0) = m_elements.at(j).at(i);
    return Matrix(elements);
}

Matrix Matrix::transpose() const
{
    std::vector<std::vector<double> > elements(m_width, std::vector<double>(m_height));
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            elements.at(j).at(i) = m_elements.at(i).at(j);
    return Matrix(elements);
}

Matrix Matrix::operator*(double scalar) const
{
    Matrix M(m_elements);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            M.at(i, j) *= scalar;
    return M;
}

Matrix operator*(double scalar, const Matrix& M)
{
    return M.operator*(scalar);
}

Matrix Matrix::operator+(double scalar) const
{
    Matrix M(m_elements);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            M.at(i, j) += scalar;
    return M;
}

Matrix operator+(double scalar, const Matrix& M)
{
    return M.operator+(scalar);
}

Matrix Matrix::operator*(const Matrix& M) const
{
    unsigned n = m_height;
    unsigned k1 = m_width;
    unsigned k2 = M.m_height;
    unsigned m = M.m_width;

    if(k1 != k2)
        throw std::invalid_argument( "Matrix dimensions do not match (A*B is defined if A.m_width == B.m_height)!" );

    Matrix R(n, m);

    for(unsigned i=0; i<n; i++)
        for(unsigned j=0; j<m; j++)
        {
            auto row = this->row(i).to_vector();
            auto col = M.col(j).transpose().to_vector();
            R.at(i, j) = std::inner_product(
                std::begin(row), std::end(row),
                std::begin(col),
                0.0
            );
        }

    return R;
}

Matrix Matrix::product(const Matrix& M) const
{
    if(m_height != M.m_height || m_width != M.m_width)
        throw std::invalid_argument( "Matrix dimensions do not match(they must be same dimensions)!" );

    Matrix R(m_height, m_width);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            R.at(i, j) = this->at(i, j)*M.at(i, j);

    return R;
}

Matrix Matrix::operator+(const Matrix& M) const
{
    if(m_height != M.m_height || m_width != M.m_width)
        throw std::invalid_argument( "Matrix dimensions do not match(they must be same dimensions)!" );

    Matrix R(m_height, m_width);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            R.at(i, j) = this->at(i, j)+M.at(i, j);

    return R;
}

Matrix Matrix::operator-(const Matrix& M) const
{
    return *this + (-1)*M;
}

Matrix Matrix::_minor(unsigned r, unsigned c) const
{
    Matrix R(m_height-1, m_width-1);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            if(i == r || j == c)
                continue;
            else
            {
                int correction_i = (i > r) ? -1 : 0;
                int correction_j = (j > c) ? -1 : 0;
                R.at(i + correction_i, j + correction_j) = this->at(i, j);
            }
    return R;
}

double Matrix::det() const
{
    if(m_height != m_width)
        throw std::invalid_argument( "Only square matrix can have inverse!" );

    unsigned n = m_height;
    if(n == 1)
        return this->at(0, 0);
    double d = 0.0;

    for(unsigned i=0; i<n; i++)
        d += this->at(0, i)*this->_minor(0, i).det() * ((i%2 == 0) ? 1 : -1);

    return d;
}

Matrix Matrix::adj() const
{
    Matrix R(m_height, m_width);
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
            R.at(i, j) = this->_minor(i, j).det() * (((i+j)%2 == 0) ? 1 : -1);

    return R.transpose();
}

Matrix Matrix::inv() const
{   
    double d = this->det();
    if(d == 0)
        throw std::invalid_argument("Given matrix is singular and inverse can't be found!");

    return this->adj() * (1.0/d);
}

Matrix Matrix::operator/(const Matrix& b) const
{
    if(b.m_width != 1)
        throw std::invalid_argument("Matrix b must have shape Nx1!");
    if(b.m_height != m_height)
        throw std::invalid_argument("Matrix b must be same height as matrix A!");
    return this->inv()*b;
}

double Matrix::norm1() const
{
    double sum = 0;
    for(auto row: m_elements)
    {
        std::transform(std::begin(row), std::end(row), std::begin(row), abs);
        sum += std::accumulate(std::begin(row), std::end(row), 0.0);
    }
    return sum;
}

bool Matrix::operator==(const Matrix& M) const
{
    return m_elements == M.m_elements;
}

bool Matrix::operator!=(const Matrix& M) const
{
    return !(*this == M);
}

Matrix Matrix::remove_column(unsigned index)
{
    if(index >= m_width)
        throw std::invalid_argument("Column index ouf of range!");
    
    std::vector<std::vector<double> > mat(m_height, std::vector<double>(m_width-1));
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
        {
            if(j == index)
                continue;
            int k = (j > index) ? 1 : 0;
            mat.at(i).at(j-k) = m_elements.at(i).at(j);
        }
    return Matrix(mat);
}

Matrix Matrix::remove_row(unsigned index)
{
    if(index >= m_height)
        throw std::invalid_argument("Row index ouf of range!");
    
    std::vector<std::vector<double> > mat(m_height-1, std::vector<double>(m_width));
    for(unsigned i=0; i<m_height; i++)
        for(unsigned j=0; j<m_width; j++)
        {
            if(i == index)
                continue;
            int k = (i > index) ? 1 : 0;
            mat.at(i-k).at(j) = m_elements.at(i).at(j);
        }
    return Matrix(mat);
}

Matrix identity(unsigned size)
{
    Matrix R(size);
    for(unsigned i=0; i<size; i++)
        R.at(i, i) = 1;
    return R;
}

void append(Matrix& A, const Matrix& B)
{
    if(A.m_height != B.m_height)
        throw std::invalid_argument("Matrices must have same height!");

    // using copy tmp instead of B because of append(A, A) shenanigans
    auto tmp = B;

    for(unsigned i=0; i<tmp.m_width; i++)
        for(unsigned j=0; j<tmp.m_height; j++)
            A.m_elements.at(j).push_back(tmp.at(j,i));
    A.m_width += tmp.m_width;
}

void swap_columns(Matrix& A, unsigned i, unsigned j)
{
    for(unsigned k=0; k<A.height(); k++)
        std::swap(A.at(k, i), A.at(k, j));
}

std::vector<std::vector<double> > Matrix::to_cpp_matrix() const
{
    return m_elements;
}