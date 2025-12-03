//
// Created by IWOFLEUR on 04.12.2025.
//
#pragma once
#ifndef MATRIX_H
#define MATRIX_H

namespace course {
    class Matrix {
    private:
        int rows_, cols_;
        bool** matrix_;
    public:
        Matrix();
        Matrix(int rows, int cols);
        Matrix(const Matrix& other);
        ~Matrix();

        int getRows() const { return rows_; }
        int getCols() const { return cols_; }

        Matrix operator=(const Matrix& rhs);
        bool& operator()(int row, int col);

    private:
        inline void allocate(int rows, int cols);
        inline void deallocate();
        inline void copy_to(bool** other) const;
    };

    bool check_value(char value);
}

#endif //MATRIX_H
