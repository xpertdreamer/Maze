//
// Created by IWOFLEUR on 04.12.2025.
//

#include <iostream>
#include <maze.h>
#include <random>

namespace course {
    void Maze::clear_gen() {
        sideLine_.clear();
        counter_ = 1;
        entrance_ = {0, 0};
        exit_ = {rows_ - 1, cols_ - 1};
    }

    bool Maze::get_random_bool() {
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        return static_cast<bool>(dist(rng));
    }

    // Merge cells into one set
    void Maze::merge_set(const int i, const int element) {
        if (i + 1 >= cols_) return;
        const auto mutableSet = sideLine_[i + 1];
        for (auto j = 0; j < cols_; j++)
            // Check if cells belong to the same set
            if (sideLine_[j] == mutableSet)
                sideLine_[j] = element;
    }

    // Fill empty cells with EMPTY marker
    void Maze::fill_empty_value() {
        sideLine_.assign(cols_, EMPTY);
    }

    // Assign unique set numbers to empty cells
    void Maze::assign_unique_set() {
        for (auto i = 0; i < cols_; i++) {
            if (sideLine_[i] == EMPTY) {
                // Assign unique set to cell
                sideLine_[i] = counter_;
                counter_++;
            }
        }
    }

    // Add right vertical walls
    void Maze::add_vertical_walls(const int row) {
        for (auto i = 0; i < cols_ - 1; i++) {
            // Don't add wall at exit position on right edge
            if (row == exit_.first && i == exit_.second && exit_.second == cols_ - 1) {
                vWalls_(row, i) = false;
                continue;
            }
            // Don't add wall at entrance position on right edge
            if (row == entrance_.first && i == entrance_.second - 1 && entrance_.second == cols_ - 1) {
                vWalls_(row, i) = false;
                continue;
            }

            // Random choice or cells already in same set
            if (const auto choice = get_random_bool(); choice == true || sideLine_[i] == sideLine_[i + 1])
                vWalls_(row, i) = true;
            else
                // Merge cells into same subset
                merge_set(i, sideLine_[i]);
        }
        // Add right wall in last column
        vWalls_(row, cols_ - 1) = true;
    }

    // Add bottom horizontal walls
    void Maze::add_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++) {
            // Don't add wall below exit (if exit is above current row)
            if (row == exit_.first - 1 && i == exit_.second && exit_.first != 0) {
                hWalls_(row, i) = false;
                continue;
            }
            // Don't add wall below entrance (if entrance is on current row)
            if (row == entrance_.first && i == entrance_.second && entrance_.first != rows_ - 1) {
                hWalls_(row, i) = false;
                continue;
            }

            // Only add wall if set has more than one cell
            if (const auto choice = get_random_bool(); calc_unique_set(sideLine_[i]) != 1 && choice == true)
                hWalls_(row, i) = true;
            else
                hWalls_(row, i) = false;
        }
    }

    // Count cells in a set
    int Maze::calc_unique_set(const int element) const {
        int uniqueSet = 0;
        for (auto i = 0; i < cols_; i++)
            if (sideLine_[i] == element)
                uniqueSet++;

        return uniqueSet;
    }

    // Count horizontal walls (open passages) for a set
    int Maze::calc_horizontal_walls(const int element, const int row) {
        int horizontalWalls = 0;
        for (auto i = 0; i < cols_; i++)
            if (sideLine_[i] == element && hWalls_(row, i) == false)
                horizontalWalls++;

        return horizontalWalls;
    }

    // Ensure each set has at least one opening to next row
    void Maze::check_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++) {
            // Skip entrance and exit cells
            if ((row == entrance_.first && i == entrance_.second && entrance_.first != rows_ - 1) ||
                (row == exit_.first - 1 && i == exit_.second && exit_.first != 0)) {
                continue;
            }

            // If set has no openings, create one
            if (calc_horizontal_walls(sideLine_[i], row) == 0)
                for (auto j = 0; j < cols_; j++) {
                    if (sideLine_[j] == sideLine_[i]) {
                        if (!((row == entrance_.first && j == entrance_.second && entrance_.first != rows_ - 1) ||
                              (row == exit_.first - 1 && j == exit_.second && exit_.first != 0))) {
                            hWalls_(row, j) = false;
                            break;
                        }
                    }
                }
        }
    }

    // Prepare cells for next row
    void Maze::prepare_new_line(const int row) {
        for (auto i = 0; i < cols_; i++)
            // Clear cells that have walls below them
            if (hWalls_(row, i) == true)
                if (!((row == entrance_.first && i == entrance_.second && entrance_.first != rows_ - 1) ||
                      (row == exit_.first - 1 && i == exit_.second && exit_.first != 0)))
                    sideLine_[i] = EMPTY;
    }

    // Add the final row
    void Maze::add_end_line() {
        assign_unique_set();
        add_vertical_walls(rows_ - 1);
    }

    // Process last row: merge all sets and close bottom
    void Maze::check_end_line() {
        for (auto i = 0; i < cols_ - 1; i++) {
            // Don't add wall at exit position on bottom row
            if (i == exit_.second && exit_.first == rows_ - 1) {
                vWalls_(rows_ - 1, i) = false;
                merge_set(i, sideLine_[i]);
                continue;
            }

            // Merge all cells in last row
            if (sideLine_[i] != sideLine_[i + 1]) {
                // Remove vertical wall
                vWalls_(rows_ - 1, i) = false;
                // Merge subsets
                merge_set(i, sideLine_[i]);
            }
            // Add horizontal walls (close bottom)
            hWalls_(rows_ - 1, i) = true;
        }

        // Close bottom row (except at exit)
        for (auto i = 0; i < cols_; i++) {
            if (exit_.first == rows_ - 1 && i == exit_.second) {
                hWalls_(rows_ - 1, i) = false;
            } else {
                hWalls_(rows_ - 1, i) = true;
            }
        }
    }

    // Main maze generation algorithm
    void Maze::generate_maze() {
        // 1. Initialize
        fill_empty_value();

        // 2-5.1. Process all rows except last
        for (auto j = 0; j < rows_ - 1; j++) {
            // 2. Assign unique sets
            assign_unique_set();
            // 3. Add vertical walls
            add_vertical_walls(j);
            // 4. Add horizontal walls
            add_horizontal_walls(j);
            check_horizontal_walls(j);
            // 5.1. Prepare next line
            prepare_new_line(j);
        }

        // 5.2. Process last row
        add_end_line();
        check_end_line();

        // Open entrance and exit by removing boundary walls
        open_entrance_exit();
    }

    // Open entrance and exit on maze boundaries
    void Maze::open_entrance_exit() {
        // Open entrance
        if (entrance_.first == 0) {
            // Top boundary - remove horizontal wall above
            hWalls_(0, entrance_.second) = false;
        } else if (entrance_.first == rows_ - 1) {
            // Bottom boundary - remove horizontal wall below
            hWalls_(rows_ - 1, entrance_.second) = false;
        }

        if (entrance_.second == cols_ - 1 && entrance_.first > 0 && entrance_.first < rows_ - 1) {
            // Right boundary (not corner) - remove vertical wall to the left
            vWalls_(entrance_.first, entrance_.second - 1) = false;
        }
        // Note: Left boundary (entrance_.second == 0) needs no wall removal as there's no internal wall

        // Open exit
        if (exit_.first == 0) {
            // Top boundary - remove horizontal wall above
            hWalls_(0, exit_.second) = false;
        } else if (exit_.first == rows_ - 1) {
            // Bottom boundary - remove horizontal wall below
            hWalls_(rows_ - 1, exit_.second) = false;
        }

        if (exit_.second == cols_ - 1 && exit_.first > 0 && exit_.first < rows_ - 1) {
            // Right boundary (not corner) - remove vertical wall to the left
            vWalls_(exit_.first, exit_.second - 1) = false;
        }
        // Note: Left boundary (exit_.second == 0) needs no wall removal as there's no internal wall
    }

    void Maze::set_entrance(int row, int col) {
        if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
            entrance_ = {row, col};
        }
    }

    void Maze::set_exit(int row, int col) {
        if (row >= 0 && row < rows_ && col >= 0 && col < cols_) {
            exit_ = {row, col};
        }
    }

    void Maze::set_sizes(const int rows, const int cols) {
        rows_ = rows;
        cols_ = cols;
        allocate_walls();
        entrance_ = {0, 0};
        exit_ = {rows_ - 1, cols_ - 1};
    }

    inline void Maze::allocate_walls() {
        vWalls_ = Matrix(rows_, cols_);
        hWalls_ = Matrix(rows_, cols_);
    }

    void Maze::parse_size() {
        std::string line;
        std::getline(mazeFile_, line);
        size_t subPos = 0;
        rows_ = std::stoi(line, &subPos);
        cols_ = std::stoi(line.substr(subPos));
        if ((rows_ < 0 || rows_ > 60) || (cols_ < 0 || cols_ > 60))
            throw std::invalid_argument("Wrong maze size");
        entrance_ = {0, 0};
        exit_ = {rows_ - 1, cols_ - 1};
    }

    void Maze::parse_walls(Matrix &walls) {
        for (auto i = 0; i < rows_; i++) {
            std::string line;
            std::getline(mazeFile_, line);
            for (auto j = 0, k = 0; j < cols_; j++, k += 2)
                walls(i, j) = check_value(line.at(k));
        }
    }

    void Maze::from_file(const std::string& filename) {
        mazeFile_ = std::ifstream(filename);
        if (!mazeFile_.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        parse_size();
        allocate_walls();
        parse_walls(vWalls_);

        std::string line;
        std::getline(mazeFile_, line);

        parse_walls(hWalls_);
        mazeFile_.close();
    }

    void Maze::print_maze() {
        std::cout << "\n";

        // Print top border
        for (int j = 0; j < cols_; j++) {
            std::cout << "+";
            if (entrance_.first == 0 && entrance_.second == j) {
                std::cout << " E ";  // Entrance on top
            } else {
                std::cout << "---";
            }
        }
        std::cout << "+\n";

        // Print maze rows
        for (int i = 0; i < rows_; i++) {
            // Left border
            if (entrance_.first == i && entrance_.second == 0) {
                std::cout << " ";  // Open entrance on left
            } else {
                std::cout << "|";
            }

            // Print cells and vertical walls
            for (int j = 0; j < cols_; j++) {
                if (entrance_.first == i && entrance_.second == j) {
                    std::cout << " E ";
                } else if (exit_.first == i && exit_.second == j) {
                    std::cout << " X ";
                } else {
                    std::cout << "   ";
                }

                if (j < cols_ - 1) {
                    if (vWalls_(i, j)) {
                        std::cout << "|";
                    } else {
                        std::cout << " ";
                    }
                } else {
                    // Right border
                    if (exit_.first == i && exit_.second == cols_ - 1) {
                        std::cout << " ";  // Open exit on right
                    } else {
                        std::cout << "|";
                    }
                }
            }
            std::cout << "\n";

            // Print horizontal walls (if not last row)
            if (i < rows_ - 1) {
                std::cout << "+";
                for (int j = 0; j < cols_; j++) {
                    if (hWalls_(i, j)) {
                        std::cout << "---+";
                    } else {
                        std::cout << "   +";
                    }
                }
                std::cout << "\n";
            }
        }

        // Print bottom border
        std::cout << "+";
        for (int j = 0; j < cols_; j++) {
            if (exit_.first == rows_ - 1 && exit_.second == j) {
                std::cout << " X +";  // Exit on bottom
            } else {
                std::cout << "---+";
            }
        }
        std::cout << "\n\n";
    }

    void Maze::to_file(const std::string &filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filename);
        }

        file << rows_ << " " << cols_ << std::endl;

        // Write vertical walls
        for (auto i = 0; i < rows_; i++) {
            for (auto j = 0; j < cols_; j++) {
                file << vWalls_(i, j) << " ";
            }
            file << std::endl;
        }
        file << std::endl;

        // Write horizontal walls
        for (auto i = 0; i < rows_; i++) {
            for (auto j = 0; j < cols_; j++) {
                file << hWalls_(i, j) << " ";
            }
            file << std::endl;
        }

        file.close();
    }

}