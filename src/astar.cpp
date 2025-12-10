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
        return std::abs(dot_a.first - dot_b.first) + std::abs(dot_a.second - dot_b.second);
    }

    bool Astar::is_valid_move(const int from_row, const int from_col,
                          const int to_row, const int to_col) const {
        // Validate target coordinates are within maze bounds
        if (to_row < 0 || to_row >= maze_.getRows() ||
            to_col < 0 || to_col >= maze_.getCols()) {
            return false;
            }

        const int dr = to_row - from_row;
        const int dc = to_col - from_col;

        // Movement must be exactly one step in cardinal direction
        const bool is_horizontal = (dr == 0 && abs(dc) == 1);
        const bool is_vertical = (abs(dr) == 1 && dc == 0);

        if (!is_horizontal && !is_vertical)
            return false; // Diagonal or multi-step movement


        // Check walls based on movement direction
        if (is_horizontal) {
            // Moving horizontally - check vertical walls
            const int min_col = std::min(from_col, to_col);
            return !maze_.get_v_walls()(from_row, min_col);
        } else { // is_vertical
            // Moving vertically - check horizontal walls
            const int min_row = std::min(from_row, to_row);
            return !maze_.get_h_walls()(min_row, from_col);
        }
    }

    std::vector<std::pair<int, int> > Astar::get_neighbors(const std::pair<int, int> &node) const {
        std::vector<std::pair<int, int>> neighbors;

        const std::vector<std::pair<int, int>> directions = {
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1}
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

    std::vector<std::pair<int, int>> Astar::find_path() {
        const auto start = maze_.get_entrance();
        const auto goal = maze_.get_exit();

        // Quick check: start equals goal
        if (start == goal) {
            path_ = {start};
            return path_;
        }

        // Data structures for A*
        std::priority_queue<Node, std::vector<Node>, std::greater<>> open_set;
        std::set<std::pair<int, int>> closed_set;
        std::map<std::pair<int, int>, std::pair<int, int>> came_from;
        std::map<std::pair<int, int>, double> g_score;

        // Initialize with start node
        g_score[start] = 0.0;
        open_set.emplace(start, 0.0, heuristic(start, goal));

        // Main A* loop
        while (!open_set.empty()) {
            Node current = open_set.top();
            open_set.pop();

            // Skip if already processed
            if (closed_set.contains(current.coord)) {
                continue;
            }

            // Check if we reached the goal
            if (current.coord == goal) {
                path_ = reconstruct_path(came_from, current.coord);
                return path_;
            }

            closed_set.insert(current.coord);

            // Explore neighbors
            for (const auto& neighbor : get_neighbors(current.coord)) {
                if (closed_set.contains(neighbor)) {
                    continue;
                }

                // Calculate tentative g-score
                double tentative_g = g_score[current.coord] + 1.0;

                // Update if we found a better path
                if (!g_score.contains(neighbor) || tentative_g < g_score[neighbor]) {
                    came_from[neighbor] = current.coord;
                    g_score[neighbor] = tentative_g;

                    // Calculate heuristic and add to open set
                    double h = heuristic(neighbor, goal);
                    open_set.emplace(neighbor, tentative_g, h);
                }
            }
        }

        // No path found
        return {};
    }

    void Astar::print_path(const std::vector<std::pair<int, int>>& path) {
        if (path.empty()) {
            std::cout << "Path is empty" << std::endl;
            return;
        }

        const auto entrance = maze_.get_entrance();
        const auto exit = maze_.get_exit();
        const int rows = maze_.getRows();
        const int cols = maze_.getCols();

        // Create display grid initialized with spaces
        std::vector<std::vector<char>> display(rows, std::vector<char>(cols, ' '));

        // Mark path with directional markers
        for (size_t i = 0; i < path.size(); i++) {
            const auto [row, col] = path[i];

            if (i == 0) {
                display[row][col] = 'E'; // Entrance marker
            } else if (i == path.size() - 1) {
                display[row][col] = 'X'; // Exit marker
            } else if (i + 1 < path.size()) {
                const auto [next_row, next_col] = path[i + 1];

                if (next_row > row)      display[row][col] = 'v';
                else if (next_row < row) display[row][col] = '^';
                else if (next_col > col) display[row][col] = '>';
                else if (next_col < col) display[row][col] = '<';
            }
        }

        std::cout << "\nMaze with A* path:\n\n";

        // Print top border with entrance marker
        for (int j = 0; j < cols; j++) {
            std::cout << "+";
            if (entrance.first == 0 && entrance.second == j) {
                std::cout << " E "; // Entrance at top border
            } else {
                std::cout << "---";
            }
        }
        std::cout << "+\n";

        // Print maze rows with walls and path markers
        for (int i = 0; i < rows; i++) {
            std::cout << "|";

            for (int j = 0; j < cols; j++) {
                std::cout << " " << display[i][j] << " ";

                // Print vertical wall if exists
                if (j < cols - 1) {
                    std::cout << (maze_.get_v_walls()(i, j) ? "|" : " ");
                } else {
                    std::cout << "|";
                }
            }
            std::cout << std::endl;

            // Print horizontal walls between rows (except after last row)
            if (i < rows - 1) {
                std::cout << "+";
                for (int j = 0; j < cols; j++) {
                    std::cout << (maze_.get_h_walls()(i, j) ? "---+" : "   +");
                }
                std::cout << std::endl;
            }
        }

        // Print bottom border with exit marker
        std::cout << "+";
        for (int j = 0; j < cols; j++) {
            if (exit.first == rows - 1 && exit.second == j) {
                std::cout << " X +"; // Exit at bottom border
            } else {
                std::cout << "---+";
            }
        }
        std::cout << "\n\n";

        std::cout << "Legend:\n";
        std::cout << "  E = Entrance\n";
        std::cout << "  X = Exit\n";

        // Print path statistics
        std::cout << "Path statistics:\n";
        std::cout << "  Path length: " << path.size() - 1 << " steps\n";
        std::cout << "  Total cells in path: " << path.size() << "\n\n";
    }

    void Astar::print_path_at(const std::vector<std::pair<int, int>>& path) {
        if (path.empty()) {
            std::cout << "Path is empty" << std::endl;
            return;
        }

        const auto entrance = maze_.get_entrance();
        const auto exit = maze_.get_exit();
        const int rows = maze_.getRows();
        const int cols = maze_.getCols();

        // Create display grid initialized with spaces
        std::vector<std::vector<char>> display(rows, std::vector<char>(cols, ' '));

        // Mark the entire path with '*'
        for (const auto& [row, col] : path) {
            display[row][col] = '*';
        }

        // Overwrite entrance and exit with special markers
        display[entrance.first][entrance.second] = 'E';
        display[exit.first][exit.second] = 'X';

        std::cout << "\nMaze with A* path:\n\n";

        // Print top border with entrance marker
        for (int j = 0; j < cols; j++) {
            std::cout << "+";
            if (entrance.first == 0 && entrance.second == j) {
                std::cout << " E "; // Entrance at top border
            } else {
                std::cout << "---";
            }
        }
        std::cout << "+\n";

        // Print maze rows with walls and path markers
        for (int i = 0; i < rows; i++) {
            std::cout << "|";

            for (int j = 0; j < cols; j++) {
                // Print cell with path marker if present
                std::cout << " " << display[i][j] << " ";

                // Print vertical wall if exists
                if (j < cols - 1) {
                    std::cout << (maze_.get_v_walls()(i, j) ? "|" : " ");
                } else {
                    std::cout << "|";
                }
            }
            std::cout << std::endl;

            // Print horizontal walls between rows (except after last row)
            if (i < rows - 1) {
                std::cout << "+";
                for (int j = 0; j < cols; j++) {
                    std::cout << (maze_.get_h_walls()(i, j) ? "---+" : "   +");
                }
                std::cout << std::endl;
            }
        }

        // Print bottom border with exit marker
        std::cout << "+";
        for (int j = 0; j < cols; j++) {
            if (exit.first == rows - 1 && exit.second == j) {
                std::cout << " X +"; // Exit at bottom border
            } else {
                std::cout << "---+";
            }
        }
        std::cout << "\n\n";

        // Print legend for clarity
        std::cout << "Legend:\n";
        std::cout << "  E = Entrance\n";
        std::cout << "  X = Exit\n";
        std::cout << "  * = Path\n\n";

        // Print path statistics
        std::cout << "Path statistics:\n";
        std::cout << "  Path length: " << path.size() - 1 << " steps\n";
        std::cout << "  Total cells in path: " << path.size() << "\n\n";
    }
}
