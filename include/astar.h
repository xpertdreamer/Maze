//
// Created by IWOFLEUR on 10.12.2025.
//

#ifndef ASTAR_H
#define ASTAR_H

#include <map>
#include <maze.h>
#include <utility>

namespace course {
    class Astar {
    private:
        struct Node {
            std::pair<int, int> coord;
            /// Cost of travel from start
            double cost;
            /// Heuristic assessment to goal
            double heuristic;
            /// cost + heuristic
            double f;

            explicit Node(const std::pair<int, int> &coord_val = {-1, -1}, const double cost_val = 0, const double heuristic_val = 0)
                : coord(coord_val), cost(cost_val), heuristic(heuristic_val), f(cost_val + heuristic_val) {}

            /// For priority queue
            bool operator>(const Node &other) const {
                return f > other.f;
            }

            bool operator==(const Node &other) const {
                return coord == other.coord;
            }
        };

    public:
        explicit Astar(Maze &maze) : maze_(std::move(maze)) {}

        std::vector<std::pair<int, int>> find_path();
        void print_path(const std::vector<std::pair<int, int>> &path);
        void print_path_at(const std::vector<std::pair<int, int>>& path);
        std::vector<std::pair<int, int>> get_path() { return path_; }

    private:
        Maze maze_;
        std::vector<std::pair<int, int>> path_;

        static double heuristic(const std::pair<int, int>& dot_a, const std::pair<int, int>& dot_b) ;
        std::vector<std::pair<int, int>> get_neighbors(const std::pair<int, int>& node) const;
        static std::vector<std::pair<int, int>> reconstruct_path(
            const std::map<std::pair<int, int>, std::pair<int, int>>& came_from,
            const std::pair<int, int>& current);
        bool is_valid_move(int from_row, int from_col, int to_row, int to_col) const;
    };
}

#endif //ASTAR_H
