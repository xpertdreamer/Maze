//
// Updated Race Mode with persistent state between command calls
//

#include "racemode.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace course {

const std::string RACE_STATE_FILE = "race_state.tmp";

// Save race state to file
void RaceMode::save_state() const {
    std::ofstream file(RACE_STATE_FILE);
    if (!file.is_open()) return;

    file << race_started_ << "\n";
    file << race_finished_ << "\n";
    file << current_position_.first << " " << current_position_.second << "\n";
    file << player_stats_.moves << "\n";
    file << player_stats_.time_seconds << "\n";
    file << player_stats_.completed << "\n";

    // Save path
    file << player_stats_.path.size() << "\n";
    for (const auto& pos : player_stats_.path) {
        file << pos.first << " " << pos.second << "\n";
    }

    // Save start time (as seconds since epoch)
    auto duration = start_time_.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    file << seconds << "\n";

    file.close();
}

// Load race state from file
void RaceMode::load_state() {
    std::ifstream file(RACE_STATE_FILE);
    if (!file.is_open()) return;

    file >> race_started_;
    file >> race_finished_;
    file >> current_position_.first >> current_position_.second;
    file >> player_stats_.moves;
    file >> player_stats_.time_seconds;
    file >> player_stats_.completed;

    // Load path
    size_t path_size;
    file >> path_size;
    player_stats_.path.clear();
    for (size_t i = 0; i < path_size; i++) {
        int row, col;
        file >> row >> col;
        player_stats_.path.emplace_back(row, col);
    }

    // Load start time
    long long seconds;
    file >> seconds;
    start_time_ = std::chrono::steady_clock::time_point(std::chrono::seconds(seconds));

    file.close();
}

// Constructor
RaceMode::RaceMode(Maze& maze) : maze_(maze) {
    current_position_ = maze_.get_entrance();
    load_state(); // Try to load existing state
}

// Start a new race
void RaceMode::start_race() {
    if (race_started_) {
        std::cout << "⚠️  Race already started!\n";
        std::cout << "Current position: (" << current_position_.first << ", " << current_position_.second << ")\n";
        std::cout << "Moves: " << player_stats_.moves << "\n";
        print_current_state();
        return;
    }

    race_started_ = true;
    race_finished_ = false;
    current_position_ = maze_.get_entrance();
    player_stats_ = PlayerStats();
    player_stats_.path.push_back(current_position_);
    astar_stats_ = AStarStats();
    start_time_ = std::chrono::steady_clock::now();

    save_state();

    std::cout << "🏁 RACE STARTED!\n";
    std::cout << "Current position: (" << current_position_.first << ", " << current_position_.second << ")\n";
    std::cout << "Goal: (" << maze_.get_exit().first << ", " << maze_.get_exit().second << ")\n";
    std::cout << "Use movement commands to navigate!\n\n";

    print_current_state();
}

// Reset the current race
void RaceMode::reset_race() {
    race_started_ = false;
    race_finished_ = false;
    current_position_ = maze_.get_entrance();
    player_stats_ = PlayerStats();
    astar_stats_ = AStarStats();

    if (std::ifstream(RACE_STATE_FILE)) {
        std::remove(RACE_STATE_FILE.c_str());
    }

    std::cout << "🔄 Race reset. Use 'race_start' to begin again.\n";
}

// Validate if a move is possible
bool RaceMode::is_valid_move(int from_row, int from_col, int to_row, int to_col) const {
    if (to_row < 0 || to_row >= maze_.getRows() ||
        to_col < 0 || to_col >= maze_.getCols()) {
        return false;
    }

    int dr = to_row - from_row;
    int dc = to_col - from_col;

    bool is_horizontal = (dr == 0 && std::abs(dc) == 1);
    bool is_vertical = (std::abs(dr) == 1 && dc == 0);

    if (!is_horizontal && !is_vertical) return false;

    if (is_horizontal) {
        int min_col = std::min(from_col, to_col);
        return !maze_.get_v_walls()(from_row, min_col);
    } else {
        int min_row = std::min(from_row, to_row);
        return !maze_.get_h_walls()(min_row, from_col);
    }
}

// Attempt to move to a new position
bool RaceMode::try_move(int new_row, int new_col, const std::string& direction) {
    if (!race_started_ || race_finished_) {
        std::cout << "❌ Race not active! Use 'race_start' to begin.\n";
        return false;
    }

    if (is_valid_move(current_position_.first, current_position_.second, new_row, new_col)) {
        current_position_ = {new_row, new_col};
        player_stats_.moves++;
        player_stats_.path.push_back(current_position_);

        save_state(); // Save state after every move

        std::cout << "✅ Moved " << direction << " to (" << new_row << ", " << new_col << ")\n";
        std::cout << "Total moves: " << player_stats_.moves << "\n\n";

        check_if_finished();

        if (!race_finished_) {
            print_current_state();
        }

        return true;
    } else {
        std::cout << "🚫 Cannot move " << direction << " - wall blocking!\n\n";
        print_current_state();
        return false;
    }
}

// Movement methods
bool RaceMode::move_up() {
    return try_move(current_position_.first - 1, current_position_.second, "UP ⬆️");
}

bool RaceMode::move_down() {
    return try_move(current_position_.first + 1, current_position_.second, "DOWN ⬇️");
}

bool RaceMode::move_left() {
    return try_move(current_position_.first, current_position_.second - 1, "LEFT ⬅️");
}

bool RaceMode::move_right() {
    return try_move(current_position_.first, current_position_.second + 1, "RIGHT ➡️");
}

// Check if player reached the exit
void RaceMode::check_if_finished() {
    if (current_position_ == maze_.get_exit()) {
        race_finished_ = true;
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time_;
        player_stats_.time_seconds = elapsed.count();
        player_stats_.completed = true;

        save_state();

        std::cout << "\n╔════════════════════════════════════════════════╗\n";
        std::cout << "║    🎉 CONGRATULATIONS! YOU WON! 🎉            ║\n";
        std::cout << "╠════════════════════════════════════════════════╣\n";
        std::cout << "║  You reached the exit!                         ║\n";
        std::cout << "║  Total moves: " << std::setw(4) << player_stats_.moves << std::string(28, ' ') << "║\n";
        std::cout << "║  Time: " << std::fixed << std::setprecision(2) << std::setw(7) << player_stats_.time_seconds << " seconds";
        std::cout << std::string(23, ' ') << "║\n";
        std::cout << "╚════════════════════════════════════════════════╝\n\n";

        std::cout << "Computing optimal path with A* algorithm...\n\n";

        run_astar();
        print_comparison();

        // Clean up state file
        if (std::ifstream(RACE_STATE_FILE)) {
            std::remove(RACE_STATE_FILE.c_str());
        }
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
        astar_stats_.moves = static_cast<int>(astar_stats_.path.size()) - 1;
        astar_stats_.time_seconds = astar_elapsed.count();
    }
}

// Print comparison results
void RaceMode::print_comparison() const {
    if (!player_stats_.completed || !astar_stats_.completed) {
        std::cout << "Cannot compare - race not completed!\n";
        return;
    }

    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║         🏁 RACE RESULTS COMPARISON 🏁          ║\n";
    std::cout << "╠════════════════════════════════════════════════╣\n";

    std::cout << "║  MOVES:                                        ║\n";
    std::cout << "║    👤 Player:  " << std::setw(4) << player_stats_.moves << " moves" << std::string(24, ' ') << "║\n";
    std::cout << "║    🤖 A*:      " << std::setw(4) << astar_stats_.moves << " moves (optimal)" << std::string(13, ' ') << "║\n";

    double efficiency = (static_cast<double>(astar_stats_.moves) / player_stats_.moves) * 100.0;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << efficiency;
    std::string efficiency_str = ss.str();

    std::cout << "║    📊 Efficiency: " << efficiency_str << "%";
    int padding = 27 - static_cast<int>(efficiency_str.length());
    std::cout << std::string(padding, ' ') << "║\n";

    if (player_stats_.moves == astar_stats_.moves) {
        std::cout << "║    ⭐⭐⭐ PERFECT! Optimal path! ⭐⭐⭐          ║\n";
    } else if (efficiency >= 90.0) {
        std::cout << "║    🌟 EXCELLENT! Very close to optimal!       ║\n";
    } else if (efficiency >= 75.0) {
        std::cout << "║    👍 GOOD! Solid performance!                 ║\n";
    } else if (efficiency >= 50.0) {
        std::cout << "║    📈 Not bad! Room for improvement!           ║\n";
    } else {
        std::cout << "║    💪 Keep practicing! You'll get better!      ║\n";
    }

    std::cout << "╠════════════════════════════════════════════════╣\n";

    std::cout << "║  TIME:                                         ║\n";
    std::cout << "║    👤 Player:  " << std::fixed << std::setprecision(3)
              << std::setw(8) << player_stats_.time_seconds << " sec"
              << std::string(18, ' ') << "║\n";
    std::cout << "║    🤖 A*:      " << std::fixed << std::setprecision(6)
              << std::setw(11) << astar_stats_.time_seconds << " sec"
              << std::string(15, ' ') << "║\n";

    double time_ratio = player_stats_.time_seconds / astar_stats_.time_seconds;
    std::stringstream ratio_ss;
    ratio_ss << std::fixed << std::setprecision(0) << time_ratio;
    std::string ratio_str = ratio_ss.str();

    std::cout << "║    ⚡ A* was " << ratio_str << "x faster";
    int time_padding = 31 - static_cast<int>(ratio_str.length());
    std::cout << std::string(time_padding, ' ') << "║\n";

    std::cout << "╠════════════════════════════════════════════════╣\n";

    int extra_moves = player_stats_.moves - astar_stats_.moves;
    if (extra_moves > 0) {
        std::string moves_str = "  Extra moves: " + std::to_string(extra_moves);
        std::cout << "║" << moves_str;
        int final_padding = 48 - static_cast<int>(moves_str.length());
        std::cout << std::string(final_padding, ' ') << "║\n";
    } else {
        std::cout << "║  Perfect navigation! 🎯                        ║\n";
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
        file << "\n";
        if (player_stats_.moves == astar_stats_.moves) {
            file << std::fixed << "⭐⭐⭐ PERFECT! Optimal path! ⭐⭐⭐\n";
        } else if (efficiency >= 90.0) {
            file << std::fixed << "🌟 EXCELLENT! Very close to optimal!\n";
        } else if (efficiency >= 75.0) {
            file << std::fixed << "👍 GOOD! Solid performance!\n";
        } else if (efficiency >= 50.0) {
            file << std::fixed << "📈 Not bad! Room for improvement!\n";
        } else {
            file << std::fixed << "💪 Keep practicing! You'll get better!\n";
        }

    }

    file.close();
    std::cout << "✅ Results saved to: " << filename << "\n";
}

// Print current maze state with player position
void RaceMode::print_current_state() const {
    const auto entrance = maze_.get_entrance();
    const auto exit = maze_.get_exit();
    const int rows = maze_.getRows();
    const int cols = maze_.getCols();

    std::cout << "\n";

    // Print top border
    for (int j = 0; j < cols; j++) {
        std::cout << "+";
        if (entrance.first == 0 && entrance.second == j) {
            std::cout << " E ";
        } else {
            std::cout << "---";
        }
    }
    std::cout << "+\n";

    // Print maze with current position marked
    for (int i = 0; i < rows; i++) {
        std::cout << "|";
        for (int j = 0; j < cols; j++) {
            if (current_position_.first == i && current_position_.second == j) {
                std::cout << " @ ";  // Current position
            } else if (entrance.first == i && entrance.second == j) {
                std::cout << " E ";
            } else if (exit.first == i && exit.second == j) {
                std::cout << " X ";
            } else {
                std::cout << "   ";
            }

            if (j < cols - 1) {
                std::cout << (maze_.get_v_walls()(i, j) ? "|" : " ");
            } else {
                std::cout << "|";
            }
        }
        std::cout << "\n";

        if (i < rows - 1) {
            std::cout << "+";
            for (int j = 0; j < cols; j++) {
                std::cout << (maze_.get_h_walls()(i, j) ? "---+" : "   +");
            }
            std::cout << "\n";
        }
    }

    // Print bottom border
    std::cout << "+";
    for (int j = 0; j < cols; j++) {
        if (exit.first == rows - 1 && exit.second == j) {
            std::cout << " X +";
        } else {
            std::cout << "---+";
        }
    }
    std::cout << "\n\n";

    // Print legend and stats
    std::cout << "Legend: @ = You, E = Entrance, X = Exit\n";
    std::cout << "Position: (" << current_position_.first << ", " << current_position_.second << ")";
    std::cout << " | Moves: " << player_stats_.moves;

    if (race_started_ && !race_finished_) {
        std::cout << " | Time: " << std::fixed << std::setprecision(1) << get_elapsed_time() << "s";
    }

    std::cout << "\n\n";
}

} // namespace course