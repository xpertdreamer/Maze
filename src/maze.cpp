//
// Created by IWOFLEUR on 04.12.2025.
//

#include <maze.h>
#include <random>

namespace course {
    bool Maze::get_random_bool() {
        std::random_device rd;
        std::default_random_engine rng(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        return static_cast<bool>(dist(rng));
    }

    // Merging cells into one set
    void Maze::merge_set(const int i, const int element) {
        const auto mutableSet = sideLine_[i + 1];
        for (auto j = 0; j < cols_; j++)
            // Checking cells for one set
            if (sideLine_[j] == mutableSet)
                sideLine_[j] = element;
    }


    void Maze::fill_empty_value() {
        for (auto i = 0; i < cols_; i++) {
            sideLine_.push_back(EMPTY);
        }
    }

    void Maze::assign_unique_set() {
        for (auto i = 0; i < cols_; i++) {
            if (sideLine_[i] == EMPTY) {
                sideLine_[i] = counter_;
                counter_++;
            }
        }
    }

    // Add right vertical wall
    void Maze::add_vertical_walls(const int row) {
        for (auto i = 0; i < cols_ - 1; i++) {
            if (const auto choice = get_random_bool(); choice == true || sideLine_[i] == sideLine_[i + 1])
                vWalls_(row, i) = true;
            else
                merge_set(i, sideLine_[i]);
        }
        vWalls_(row, cols_ - 1) = true;
    }

    void Maze::add_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++) {
            if (const auto choice = get_random_bool(); calc_unique_set(sideLine_[i]) != 1 && choice == true)
                hWalls_(row, i) = true;
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

    void Maze::check_horizontal_walls(const int row) {
        for (auto i = 0; i < cols_; i++)
            if (calc_horizontal_walls(sideLine_[i], row == 0))
                hWalls_(row, i) = false;
    }

    void Maze::prepare_new_line(const int row) {
        for (auto i = 0; i < cols_; i++)
            if (hWalls_(row, i) == true)
                sideLine_[i] = EMPTY;
    }

    void Maze::add_end_line() {
        assign_unique_set();
        add_vertical_walls(rows_ - 1);

    }

    // Checking conditions for adding the last line
    void Maze::check_end_line() {
        for (auto i = 0; i < cols_; i++) {
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
        // Add horizontal wall in the last cell
        hWalls_(rows_ - 1, cols_ - 1) = true;
    }


}
