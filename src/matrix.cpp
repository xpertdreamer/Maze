//
// Created by IWOFLEUR on 04.12.2025.
//
#include <algorithm>
#include <matrix.h>
#include <stdexcept>

namespace course {
    Matrix::Matrix() : rows_(0), cols_(0), matrix_(nullptr) {}

    Matrix::Matrix(const int rows, const int cols) {
        allocate(rows, cols);
    }

    Matrix::Matrix(const Matrix& other) : Matrix(other.rows_, other.cols_){ copy_to(other.matrix_); }

    Matrix Matrix::operator=(const Matrix& rhs) {
        if (this != &rhs) {
            deallocate();
            allocate(rhs.rows_, rhs.cols_);
            copy_to(rhs.matrix_);
        }
        return *this;
    }

    bool &Matrix::operator()(const int row, const int col) { return matrix_[row][col]; }

    void Matrix::allocate(const int rows, const int cols) {
        rows_ = rows;
        cols_ = cols;
        matrix_ = new bool *[rows];
        for (int i = 0; i < rows; i++)
            matrix_[i] = new bool[cols]();
    }

    void Matrix::deallocate() {
        if (matrix_ != nullptr) {
            for (int i = 0; i < rows_; i++)
                delete[] matrix_[i];
            delete[] matrix_;
            matrix_ = nullptr;
        }
    }

    void Matrix::copy_to(bool **other) const {
        for (int i = 0; i < rows_; i++)
            for (int j = 0; j < cols_; j++)
                matrix_[i][j] = other[i][j];
    }

    bool check_value(const char value) {
        if (value != '1' && value != '0')
            throw std::invalid_argument("Invalid value");
        return value == '1';
    }

}
