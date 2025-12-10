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
        probability = 0.5;
    }

    bool Maze::get_random_bool() {
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::uniform_int_distribution<double> dist(0, 1);

        if (const double rand_num = dist(rng); rand_num < INC) {
            probability = std::min(probability + INC, 1.0);
            return true;
        }

        return false;
    }

    // Merging cells into one set
    void Maze::merge_set(const int i, const int element) {
        if (i + 1 >= cols_) return;
        const auto mutableSet = sideLine_[i + 1];
        for (auto j = 0; j < cols_; j++)
            // Checking cells for one set
            if (sideLine_[j] == mutableSet)
                sideLine_[j] = element;
    }


    void Maze::fill_empty_value() {
        sideLine_.assign(cols_, EMPTY);
    }

    // To cells
    void Maze::assign_unique_set() {
        for (auto i = 0; i < cols_; i++) {
            if (sideLine_[i] == EMPTY) {
                // Assign unique set to cell
                sideLine_[i] = counter_;
                counter_++;
            }
        }
    }

    // Add right vertical wall
    void Maze::add_vertical_walls(const int row) {
        for (auto i = 0; i < cols_ - 1; i++) {
            if (row == exit_.first && i == exit_.second) {
                vWalls_(row, i) = false;
                continue;
            }
            if (row == entrance_.first && i == entrance_.second - 1) {
                vWalls_(row, i) = false;
                continue;
            }

            // A condition to prevent looping
            if (const auto choice = get_random_bool(); choice == true || sideLine_[i] == sideLine_[i + 1])
                vWalls_(row, i) = true;
            else
                // Merge cells to subset
                merge_set(i, sideLine_[i]);
        }
        // Add right wall in last cell
        vWalls_(row, cols_ - 1) = true;
    }

    // Add bottom wall
    void Maze::add_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++) {
            if (row == exit_.first - 1 && i == exit_.second) {
                hWalls_(row, i) = false;
                continue;
            }
            if (row == 0 && i == entrance_.second) {
                hWalls_(row, i) = false;
                continue;
            }

            // The condition that the set has more than one cell
            if (const auto choice = get_random_bool(); calc_unique_set(sideLine_[i]) != 1 && choice == true)
                hWalls_(row, i) = true;
            else
                hWalls_(row, i) = false;
        }
    }

    int Maze::calc_unique_set(const int element) const {
        int uniqueSet = 0;
        for (auto i = 0; i < cols_; i++)
            if (sideLine_[i] == element)
                uniqueSet++;

        return uniqueSet;
    }

    int Maze::calc_horizontal_walls(const int element, const int row) {
        int horizontalWalls = 0;
        for (auto i = 0; i < cols_; i++)
            if (sideLine_[i] == element && hWalls_(row, i) == false)
                horizontalWalls++;

        return horizontalWalls;
    }

    // 4.1 and 4.2
    void Maze::check_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++) {
            if ((row == 0 && i == entrance_.second) ||
                (row == exit_.first - 1 && i == exit_.second)) {
                continue;
            }

            if (calc_horizontal_walls(sideLine_[i], row) == 0)
                for (auto j = 0; j < cols_; j++) {
                    if (sideLine_[j] == sideLine_[i]) {
                        hWalls_(row, j) = false;
                        break;
                    }
                }
        }
    }

    void Maze::prepare_new_line(const int row) {
        for (auto i = 0; i < cols_; i++)
            if (hWalls_(row, i) == true)
                if (!((row == 0 && i == entrance_.second) ||
                 (row == exit_.first - 1 && i == exit_.second)))
                    sideLine_[i] = EMPTY;
    }

    void Maze::add_end_line() {
        assign_unique_set();
        add_vertical_walls(rows_ - 1);

    }

    // Checking conditions for adding the last line
    void Maze::check_end_line() {
        for (auto i = 0; i < cols_ - 1; i++) {
            if (i == exit_.second && exit_.first == rows_ - 1) {
                vWalls_(rows_ - 1, i) = false;
                merge_set(i, sideLine_[i]);
                continue;
            }

            // 5.2.1
            if (sideLine_[i] != sideLine_[i + 1]) {
                // Remove vertical wall
                vWalls_(rows_ - 1, i) = false;
                // Merge subsets
                merge_set(i, sideLine_[i]);
            }
            // Add horizontal walls
            hWalls_(rows_ - 1, i) = true;
        }
        // Add horizontal wall except for exit position
        for (auto i = 0; i < cols_; i++) {
            if (exit_.first == rows_ - 1 && i == exit_.second) {
                hWalls_(rows_ - 1, i) = false;
            } else {
                hWalls_(rows_ - 1, i) = true;
            }
        }
    }

    void Maze::generate_maze() {
        // 1
        fill_empty_value();
        for (auto j = 0; j < rows_ - 1; j++) {
            // 2
            assign_unique_set();
            // 3
            add_vertical_walls(j);
            // 4
            add_horizontal_walls(j);
            check_horizontal_walls(j);
            // 5.1
            prepare_new_line(j);
        }
        // 5.2
        add_end_line();
        check_end_line();

        if (entrance_.first == 0) {
            hWalls_(0, entrance_.second) = false;
        }

        if (exit_.first == rows_ - 1) {
            hWalls_(rows_ - 1, exit_.second) = false;
        }
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
        cols_ = std::stoi(line, &subPos);
        if ((rows_ < 0 || rows_ > 60) && (cols_ < 0 || cols_ > 60))
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
        parse_size();
        allocate_walls();
        parse_walls(vWalls_);

        std::string line;
        std::getline(mazeFile_, line);

        parse_walls(hWalls_);
    }

    // void Maze::print_maze() {
    //     std::cout << std::endl;
    //     for (auto j = 0; j < cols_; j++) {
    //         if (entrance_.first == 0 && entrance_.second == j) {
    //             std::cout << "  ";
    //         } else {
    //             std::cout << " _";
    //         }
    //     }
    //     std::cout << std::endl;
    //
    //     for (auto i = 0; i < rows_; i++) {
    //         if (entrance_.first == i && entrance_.second == 0) {
    //             std::cout << " ";
    //         } else {
    //             std::cout << "|";
    //         }
    //
    //         for (auto j = 0; j < cols_; j++) {
    //             char cell = ' ';
    //             if (i < rows_ - 1 && hWalls_(i, j) == true || i == rows_ - 1) {
    //                 cell = '_';
    //             }
    //             std::cout << cell;
    //
    //             if (exit_.first == i && exit_.second == j && j == cols_ - 1) {
    //                 std::cout << ' ';
    //             } else if (j < cols_ - 1 && vWalls_(i, j) == true || j == cols_ - 1) {
    //                 std::cout << '|';
    //             } else {
    //                 std::cout << ' ';
    //             }
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    void Maze::print_maze() {
        std::cout << "\n";

        for (int j = 0; j < cols_; j++) {
            std::cout << "+";
            if (entrance_.first == 0 && entrance_.second == j) {
                std::cout << " E ";
            } else {
                std::cout << "---";
            }
        }
        std::cout << "+\n";

        for (int i = 0; i < rows_; i++) {
            std::cout << "|";
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
                    std::cout << "|";
                }
            }
            std::cout << "\n";

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

        std::cout << "+";
        for (int j = 0; j < cols_; j++) {
            if (exit_.first == rows_ - 1 && exit_.second == j) {
                std::cout << " X +";
            } else {
                std::cout << "---+";
            }
        }
        std::cout << "\n\n";
    }

    void Maze::to_file(const std::string &filename) {
        std::ofstream file(filename);
        file << rows_ << " " << cols_ << std::endl;

        for (auto i = 0; i < rows_; i++) {
            for (auto j = 0; j < cols_; j++) {
                file << vWalls_(i, j) << " ";
            }
            file << std::endl;
        }
        file << std::endl;

        for (auto i = 0; i < rows_; i++) {
            for (auto j = 0; j < cols_; j++) {
                file << hWalls_(i, j) << " ";
            }
            file << std::endl;
        }

        std::cout << "Maze to " << filename << std::endl;
    }

}
