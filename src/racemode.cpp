//
// Created for Race Mode functionality
// Complete implementation of race mode for maze solver
//

#include "racemode.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

namespace course {

// Constructor
RaceMode::RaceMode(Maze& maze) : maze_(maze) {
    current_position_ = maze_.get_entrance();
}

// Start a new race
void RaceMode::start_race() {
    if (race_started_) {
        std::cout << "Race already started! Use 'reset' to restart.\n";
        return;
    }

    race_started_ = true;
    race_finished_ = false;
    current_position_ = maze_.get_entrance();
    player_stats_ = PlayerStats();
    player_stats_.path.push_back(current_position_); // Add starting position to path
    astar_stats_ = AStarStats();
    start_time_ = std::chrono::steady_clock::now();

    std::cout << "🏁 RACE STARTED!\n";
    std::cout << "Current position: (" << current_position_.first << ", " << current_position_.second << ")\n";
    std::cout << "Goal: (" << maze_.get_exit().first << ", " << maze_.get_exit().second << ")\n";
    std::cout << "Use movement commands (up/down/left/right) to navigate!\n\n";

    print_current_state();
}

// Reset the current race
void RaceMode::reset_race() {
    race_started_ = false;
    race_finished_ = false;
    current_position_ = maze_.get_entrance();
    player_stats_ = PlayerStats();
    astar_stats_ = AStarStats();
    std::cout << "Race reset. Use 'start' to begin again.\n";
}

// Validate if a move is possible (no walls blocking)
bool RaceMode::is_valid_move(int from_row, int from_col, int to_row, int to_col) const {
    // Check bounds
    if (to_row < 0 || to_row >= maze_.getRows() ||
        to_col < 0 || to_col >= maze_.getCols()) {
        return false;
    }

    int dr = to_row - from_row;
    int dc = to_col - from_col;

    // Only cardinal directions, one step at a time
    bool is_horizontal = (dr == 0 && std::abs(dc) == 1);
    bool is_vertical = (std::abs(dr) == 1 && dc == 0);

    if (!is_horizontal && !is_vertical) return false;

    // Check walls based on direction
    if (is_horizontal) {
        // Moving left/right - check vertical walls
        int min_col = std::min(from_col, to_col);
        return !maze_.get_v_walls()(from_row, min_col);
    } else {
        // Moving up/down - check horizontal walls
        int min_row = std::min(from_row, to_row);
        return !maze_.get_h_walls()(min_row, from_col);
    }
}

// Attempt to move to a new position
bool RaceMode::try_move(int new_row, int new_col) {
    if (!race_started_ || race_finished_) {
        std::cout << "Race not active! Use 'race_start' to begin.\n";
        return false;
    }

    if (is_valid_move(current_position_.first, current_position_.second, new_row, new_col)) {
        current_position_ = {new_row, new_col};
        player_stats_.moves++;
        player_stats_.path.push_back(current_position_);

        std::cout << "✓ Moved to (" << new_row << ", " << new_col << ") - Moves: " << player_stats_.moves << "\n";
        check_if_finished();
        return true;
    } else {
        std::cout << "✗ Invalid move - wall blocking!\n";
        return false;
    }
}

// Movement methods
bool RaceMode::move_up() {
    return try_move(current_position_.first - 1, current_position_.second);
}

bool RaceMode::move_down() {
    return try_move(current_position_.first + 1, current_position_.second);
}

bool RaceMode::move_left() {
    return try_move(current_position_.first, current_position_.second - 1);
}

bool RaceMode::move_right() {
    return try_move(current_position_.first, current_position_.second + 1);
}

// Check if player reached the exit
void RaceMode::check_if_finished() {
    if (current_position_ == maze_.get_exit()) {
        race_finished_ = true;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time_;
        player_stats_.time_seconds = elapsed.count();
        player_stats_.completed = true;

        std::cout << "\n🎉 CONGRATULATIONS! You reached the exit!\n";
        std::cout << "Total moves: " << player_stats_.moves << "\n";
        std::cout << "Time: " << std::fixed << std::setprecision(2) << player_stats_.time_seconds << " seconds\n";
        std::cout << "\nRunning A* algorithm for comparison...\n";

        run_astar();
        print_comparison();
    }
}

// Get elapsed time in race
double RaceMode::get_elapsed_time() const {
    if (!race_started_) return 0.0;
    if (race_finished_) return player_stats_.time_seconds;

    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = current_time - start_time_;
    return elapsed.count();
}

// Run A* algorithm for comparison
void RaceMode::run_astar() {
    auto astar_start = std::chrono::steady_clock::now();

    Astar astar(maze_);
    astar_stats_.path = astar.find_path();

    auto astar_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> astar_elapsed = astar_end - astar_start;

    if (!astar_stats_.path.empty()) {
        astar_stats_.completed = true;
        astar_stats_.moves = static_cast<int>(astar_stats_.path.size()) - 1; // -1 because path includes start
        astar_stats_.time_seconds = astar_elapsed.count();
    }
}

// Format percentage helper
std::string RaceMode::format_percentage(double value, double reference) const {
    if (reference == 0) return "N/A";
    double percentage = (value / reference) * 100.0;
    return std::to_string(static_cast<int>(percentage)) + "%";
}

// Print comparison results in a fun format
void RaceMode::print_comparison() const {
    if (!player_stats_.completed || !astar_stats_.completed) {
        std::cout << "Cannot compare - race not completed!\n";
        return;
    }

    std::cout << "\n╔════════════════════════════════════════════════╗\n";
    std::cout << "║         🏁 RACE RESULTS COMPARISON 🏁          ║\n";
    std::cout << "╠════════════════════════════════════════════════╣\n";

    // Moves comparison
    std::cout << "║  MOVES:                                        ║\n";
    std::cout << "║    👤 Player:  " << std::setw(4) << player_stats_.moves << " moves" << std::string(24, ' ') << "║\n";
    std::cout << "║    🤖 A*:      " << std::setw(4) << astar_stats_.moves << " moves (optimal)" << std::string(13, ' ') << "║\n";

    double efficiency = (static_cast<double>(astar_stats_.moves) / player_stats_.moves) * 100.0;
    std::string efficiency_str = std::to_string(static_cast<int>(efficiency * 10) / 10.0);
    efficiency_str = efficiency_str.substr(0, efficiency_str.find('.') + 2); // Keep 1 decimal

    std::cout << "║    📊 Your efficiency: " << efficiency_str << "%";
    int padding = 22 - static_cast<int>(efficiency_str.length());
    std::cout << std::string(padding, ' ') << "║\n";

    // Fun rating based on efficiency
    if (player_stats_.moves == astar_stats_.moves) {
        std::cout << "║    ⭐ PERFECT! Optimal path found!            ║\n";
    } else if (efficiency >= 90.0) {
        std::cout << "║    🌟 EXCELLENT! Very close to optimal!       ║\n";
    } else if (efficiency >= 75.0) {
        std::cout << "║    👍 GOOD! Decent performance!                ║\n";
    } else if (efficiency >= 50.0) {
        std::cout << "║    📈 Not bad, but room for improvement!       ║\n";
    } else {
        std::cout << "║    💪 Keep practicing!                         ║\n";
    }

    std::cout << "╠════════════════════════════════════════════════╣\n";

    // Time comparison
    std::cout << "║  TIME:                                         ║\n";
    std::cout << "║    👤 Player:  " << std::fixed << std::setprecision(3)
              << std::setw(8) << player_stats_.time_seconds << " sec"
              << std::string(18, ' ') << "║\n";
    std::cout << "║    🤖 A*:      " << std::fixed << std::setprecision(6)
              << std::setw(11) << astar_stats_.time_seconds << " sec"
              << std::string(15, ' ') << "║\n";

    double time_ratio = player_stats_.time_seconds / astar_stats_.time_seconds;
    std::string ratio_str = std::to_string(static_cast<int>(time_ratio));
    std::cout << "║    ⚡ A* was " << ratio_str << "x faster!";
    int time_padding = 30 - static_cast<int>(ratio_str.length());
    std::cout << std::string(time_padding, ' ') << "║\n";

    std::cout << "╠════════════════════════════════════════════════╣\n";

    // Extra moves summary
    int extra_moves = player_stats_.moves - astar_stats_.moves;
    if (extra_moves > 0) {
        std::string moves_str = "  You made " + std::to_string(extra_moves) + " extra move" +
                                (extra_moves != 1 ? "s" : "");
        std::cout << "║" << moves_str;
        int final_padding = 48 - static_cast<int>(moves_str.length());
        std::cout << std::string(final_padding, ' ') << "║\n";
    } else {
        std::cout << "║  You matched the optimal path! 🎯             ║\n";
    }

    std::cout << "╚════════════════════════════════════════════════╝\n\n";
}

// Save results to file
void RaceMode::save_results_to_file(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout << "Error: Could not save results to file!\n";
        return;
    }

    file << "=== RACE MODE RESULTS ===\n\n";
    file << "Player Statistics:\n";
    file << "  Moves: " << player_stats_.moves << "\n";
    file << "  Time: " << std::fixed << std::setprecision(3) << player_stats_.time_seconds << " seconds\n";
    file << "  Completed: " << (player_stats_.completed ? "Yes" : "No") << "\n\n";

    file << "A* Algorithm Statistics:\n";
    file << "  Moves: " << astar_stats_.moves << "\n";
    file << "  Time: " << std::fixed << std::setprecision(6) << astar_stats_.time_seconds << " seconds\n";
    file << "  Completed: " << (astar_stats_.completed ? "Yes" : "No") << "\n\n";

    if (player_stats_.completed && astar_stats_.completed) {
        double efficiency = (static_cast<double>(astar_stats_.moves) / player_stats_.moves) * 100.0;
        file << "Performance:\n";
        file << "  Efficiency: " << std::fixed << std::setprecision(1) << efficiency << "%\n";
        file << "  Extra moves: " << (player_stats_.moves - astar_stats_.moves) << "\n";
        file << "  Time ratio: " << std::fixed << std::setprecision(1)
             << (player_stats_.time_seconds / astar_stats_.time_seconds) << "x slower\n";
    }

    file.close();
    std::cout << "✅ Results saved to: " << filename << "\n";
}

// Print current maze state with player position marked
void RaceMode::print_current_state() const {
    const auto entrance = maze_.get_entrance();
    const auto exit = maze_.get_exit();
    const int rows = maze_.getRows();
    const int cols = maze_.getCols();

    std::cout << "\n";

    // Print top border with entrance marker
    for (int j = 0; j < cols; j++) {
        std::cout << "+";
        if (entrance.first == 0 && entrance.second == j) {
            std::cout << " E ";
        } else {
            std::cout << "---";
        }
    }
    std::cout << "+\n";

    // Print maze rows with current position
    for (int i = 0; i < rows; i++) {
        std::cout << "|";
        for (int j = 0; j < cols; j++) {
            // Mark current position with @ symbol
            if (current_position_.first == i && current_position_.second == j) {
                std::cout << " @ ";
            } else if (entrance.first == i && entrance.second == j) {
                std::cout << " E ";
            } else if (exit.first == i && exit.second == j) {
                std::cout << " X ";
            } else {
                std::cout << "   ";
            }

            // Print vertical walls
            if (j < cols - 1) {
                std::cout << (maze_.get_v_walls()(i, j) ? "|" : " ");
            } else {
                std::cout << "|";
            }
        }
        std::cout << "\n";

        // Print horizontal walls
        if (i < rows - 1) {
            std::cout << "+";
            for (int j = 0; j < cols; j++) {
                std::cout << (maze_.get_h_walls()(i, j) ? "---+" : "   +");
            }
            std::cout << "\n";
        }
    }

    // Print bottom border with exit marker
    std::cout << "+";
    for (int j = 0; j < cols; j++) {
        if (exit.first == rows - 1 && exit.second == j) {
            std::cout << " X +";
        } else {
            std::cout << "---+";
        }
    }
    std::cout << "\n\n";

    // Print legend and current stats
    std::cout << "Legend: @ = Your position, E = Entrance, X = Exit\n";
    std::cout << "Current position: (" << current_position_.first << ", "
              << current_position_.second << ")\n";
    std::cout << "Moves so far: " << player_stats_.moves << "\n";

    if (race_started_ && !race_finished_) {
        std::cout << "Time elapsed: " << std::fixed << std::setprecision(1)
                  << get_elapsed_time() << " seconds\n";
    }

    std::cout << "\n";
}

} // namespace course