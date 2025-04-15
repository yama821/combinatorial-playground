#include <vector>
#include <assert.h>

template<typename T>
class Matrix{
    size_t cols_;
    size_t rows_;
    // std::vector<std::vector<T>> mat_;
    std::vector<T> data_;

public:
    Matrix() : cols_(0), rows_(0){}
    Matrix(size_t size): rows_(size), cols_(size), data_(size * size) {}
    Matrix(size_t rows, size_t cols): rows_(rows), cols_(cols), data_(rows * cols) {}

    T& at(size_t row, size_t col) {
        return data_.at(row * cols_ + col);
    }

    const T& at(size_t row, size_t col) const {
        return data_.at(row * cols_ + col);
    }

    Matrix<T>& operator+=(const Matrix<T>& other) {
        assert(this->rows() == other.rows());
        assert(this->cols() == other.cols());
        for (size_t row = 0; row < rows_; ++row) {
            for (size_t col = 0; col < cols_; ++col) {
                this->at(row, col) += other.at(row, col);
            }
        }
        return (*this);
    }

    Matrix<T>& operator-=(const Matrix<T>& other) {
        assert(this->rows() == other.rows());
        assert(this->cols() == other.cols());
        for (size_t row = 0; row < rows_; ++row) {
            for (size_t col = 0; col < cols_; ++col) {
                this->at(row, col) -= other.at(row, col);
            }
        }
        return (*this);
    }

    Matrix<T>& operator*=(const Matrix<T>& other) {
        assert(this->cols() == other.rows());
        size_t next_rows = rows(), next_cols = other.cols();
        std::vector<T> next_data(next_rows * next_cols, 0);
        for (size_t row = 0; row < next_rows; ++row) {
            for (size_t col = 0; col < next_cols; ++col) {
                for (size_t k = 0; k < cols(); ++k) {
                    next_data.at(row * next_cols + col) += this->at(row, k) * other.at(k, col);
                }
            }
        }
        std::swap(rows_, next_rows);
        std::swap(cols_, next_cols);
        std::swap(data_, next_data);

        return (*this);
    }

    Matrix<T> operator+(const Matrix<T>& other) const {
        Matrix<T> result(*this);
        result += other;
        return result;
    }
    Matrix<T> operator-(const Matrix<T>& other) const {
        Matrix<T> result(*this);
        result -= other;
        return result;
    }
    Matrix<T> operator*(const Matrix<T>& other) const {
        Matrix<T> result(*this);
        result *= other;
        return result;
    }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

};
