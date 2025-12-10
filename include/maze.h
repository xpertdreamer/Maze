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
        std::pair<int, int> entrance_;
        std::pair<int, int> exit_;

    public:
        int getRows() const { return rows_; }
        int getCols() const { return cols_; }
        Matrix& get_h_walls() {return hWalls_;}
        Matrix& get_v_walls() {return vWalls_;}
        Matrix get_h_walls() const  {return hWalls_;}
        Matrix get_v_walls() const {return vWalls_;}
        auto get_entrance() const { return entrance_; }
        auto get_exit() const { return exit_; }

        void set_entrance(int row, int col);
        void set_exit(int row, int col);
        void set_sizes(int rows, int cols);
        void from_file(const std::string& filename);
        static bool get_random_bool();
        void generate_maze();
        void print_maze();
        void clear_gen();
        void to_file(const std::string& filename);

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

        inline void allocate_walls();
        void parse_size();
        void parse_walls(Matrix& walls);
    };
}

#endif //MAZE_H
