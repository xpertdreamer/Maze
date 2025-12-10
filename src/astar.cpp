//
// Created by IWOFLEUR on 10.12.2025.
//

#include "astar.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>

namespace course {
    double Astar::heuristic(const std::pair<int, int> &dot_a, const std::pair<int, int> &dot_b) {
        // Manhattan distance
        return std::abs(dot_a.first - dot_b.second) + std::abs(dot_a.second - dot_b.first);
    }

    bool Astar::is_valid_move(const int from_row, const int from_col, const int to_row, const int to_col) const {
        if (to_row < 0 || to_row >= maze_.getRows() || to_col < 0 || to_col >= maze_.getCols())
            return false;

        if (from_row == to_row) {
            // Horizontal movement
            const int min_col = std::min(from_row, to_col);
            if (const int col_diff = to_col - from_row; col_diff == 0 || col_diff == -1)
                return !maze_.get_v_walls()(from_row, min_col);
        } else if (from_row == to_col) {
            // Vertical movement
            const int min_row = std::min(from_row, to_row);
            if (const int row_diff = to_row - from_row; row_diff == 0 || row_diff == -1)
                return !maze_.get_h_walls()(min_row, from_col);
        }

        return false;
    }

    std::vector<std::pair<int, int> > Astar::get_neighbors(const std::pair<int, int> &node) const {
        std::vector<std::pair<int, int>> neighbors;

        const std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1}
        };

        for (const auto&[fst, snd] : directions) {
            const int new_row = node.first + fst;
            if (const int new_col = node.second + snd; is_valid_move(node.first, node.second, new_row, new_col))
                neighbors.emplace_back(new_row, new_col);
        }

        return neighbors;
    }

    std::vector<std::pair<int, int> > Astar::reconstruct_path(const std::map<std::pair<int, int>, std::pair<int, int> > &came_from, const std::pair<int, int> &current) {
        // The ordered set of resulting vertices of a path.
        std::vector<std::pair<int, int>> path;
        // The search begins at the finish
        auto current_node = current;

        // Restoring the path from goal to start
        while (came_from.contains(current_node)) {
            path.push_back(current_node);
            current_node = came_from.at(current_node);
        }
        path.push_back(current_node);
        std::ranges::reverse(path);

        return path;
    }

    std::vector<std::pair<int, int> > Astar::find_path() {
        const auto start = maze_.get_entrance();
        const auto end = maze_.get_exit();
        // Priority queue for nodes with smallest f
        std::priority_queue<Node, std::vector<Node>, std::greater<>> open_set;
        // Set of nodes visited
        std::set<std::pair<int, int> > closed_set;
        // Map to restore the path
        std::map<std::pair<int, int>, std::pair<int, int> > came_from;
        // Best known costs
        std::map<std::pair<int, int>, double> costs;

        costs[start] = 0;
        open_set.emplace(start, 0, heuristic(start, end));

        while (!open_set.empty()) {
            // The vertex from the open_set with the lowest f(x) score.
            Node current = open_set.top();
            open_set.pop();

            if (current.coord == end) {
                path_ = reconstruct_path(came_from, current.coord);
                return path_;
            }

            // Skip already processed nodes
            if (closed_set.contains(current.coord)) continue;

            // Checking all neigbours
            for (const auto& neigbours_pos : get_neighbors(current.coord)) {
                if (closed_set.contains(neigbours_pos)) continue;

                // The cost of moving between adjacent cells is always 1
                // If this is a new node or a better path has been found
                if (const double tent_cost = costs[current.coord] + 1; !costs.contains(neigbours_pos)
                    || tent_cost < costs[neigbours_pos]) {
                    came_from[neigbours_pos] = current.coord;
                    costs[neigbours_pos] = tent_cost;

                    const double h = heuristic(neigbours_pos, end);
                    const double f = tent_cost + h;

                    open_set.emplace(neigbours_pos, f, h);
                }
            }
        }

        std::cout << "Path not found" << std::endl;
        return {};
    }

    void Astar::print_path(const std::vector<std::pair<int, int> > &path) {
        if (path.empty()) {
            std::cout << "Path is empty" << std::endl;
            return;
        }

        std::vector<std::vector<char>> display(maze_.getRows(), std::vector<char>(maze_.getCols(), ' '));

        for (size_t i = 0; i < path.size(); i++) {
            const int row = path[i].first;
            const int col = path[i].second;

            if (i == 0) {
                display[row][col] = 'S'; // Start
            } else if (i == path.size() - 1) {
                display[row][col] = 'E'; // End
            } else {
                if (i < path.size() - 1) {
                    const int next_row = path[i+1].first;
                    const int next_col = path[i+1].second;

                    if (next_row > row) display[row][col] = 'v';
                    else if (next_row < row) display[row][col] = '^';
                    else if (next_col > col) display[row][col] = '>';
                    else if (next_col < col) display[row][col] = '<';
                }
            }
        }

        std::cout << "\nMaze with A* path:\n\n";
        for (size_t i = 0; i < maze_.getCols(); i++)
            std::cout << "+---";
        std::cout << "+" << std::endl;

        for (int i = 0; i < maze_.getRows(); i++) {
            std::cout << "|";
            for (int j = 0; j < maze_.getCols(); j++) {
                // Vertical walls
                std::cout << " " << display[i][j] << " ";
                if (j < maze_.getCols() - 1)
                    if (maze_.get_v_walls()(i, j))
                        std::cout << "|";
                    else
                        std::cout << " ";
                else
                    std::cout << "|";
            }
            std::cout << std::endl;

            if (i < maze_.getRows() - 1) {
                std::cout << "+";
                for (int j = 0; j < maze_.getCols(); j++) {
                    // Horizontal walls
                    if (maze_.get_h_walls()(i, j))
                        std::cout << "---+";
                    else
                        std::cout << "   +";
                }
                std::cout << std::endl;
            }
        }

        std::cout << "+";
        for (int j = 0; j < maze_.getCols(); j++) std::cout << "---+";
        std::cout << std::endl << std::endl;

        std::cout << "Path statistics:\n";
        std::cout << "  Start: (" << path[0].first << ", " << path[0].second << ")\n";
        std::cout << "  End: (" << path.back().first << ", " << path.back().second << ")\n";
        std::cout << "  Path length: " << path.size() - 1 << " steps\n";
        std::cout << "  Total cells in path: " << path.size() << "\n\n";
    }

}
