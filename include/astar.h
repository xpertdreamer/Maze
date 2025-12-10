//
// Created by IWOFLEUR on 10.12.2025.
//

#ifndef ASTAR_H
#define ASTAR_H

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

    private:
        Maze maze_;
        std::vector<std::pair<int, int>> path_;
    };
}

#endif //ASTAR_H
