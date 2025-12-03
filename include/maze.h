//
// Created by IWOFLEUR on 04.12.2025.
//
#pragma once
#ifndef MAZE_H
#define MAZE_H

#define EMPTY 0

#include <fstream>
#include <iosfwd>
#include <matrix.h>
#include <vector>

namespace course {
    class Maze {
    private:
        int rows_{0}, cols_{0};
        Matrix vWalls_, hWalls_;
        std::ifstream mazeFile_;
        std::vector<int> sideLine_;
        int counter_{0};

    public:
        int getRows() const { return rows_; }
        int getCols() const { return cols_; }
        static bool get_random_bool();

    private:
        void fill_empty_value();
        void assign_unique_set();
        void add_vertical_walls(int row);
        void merge_set(int i, int element);
        void add_horizontal_walls(int row);
        int calc_unique_set(int element) const;
        void check_horizontal_walls(int row);
        int calc_horizontal_walls(int element ,int row);
        void prepare_new_line(int row);
        void add_end_line();
        void check_end_line();
    };
}

#endif //MAZE_H
