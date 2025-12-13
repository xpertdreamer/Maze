#ifndef RACEMODE_H
#define RACEMODE_H

#include <vector>
#include <string>
#include <chrono>
#include "maze.h"
#include "astar.h"

namespace course {

struct PlayerStats {
    int moves = 0;
    double time_seconds = 0.0;
    std::vector<std::pair<int, int>> path;
    bool completed = false;
};

struct AStarStats {
    int moves = 0;
    double time_seconds = 0.0;
    std::vector<std::pair<int, int>> path;
    bool completed = false;
};

class RaceMode {
private:
    Maze& maze_;
    std::pair<int, int> current_position_;
    std::chrono::steady_clock::time_point start_time_;
    PlayerStats player_stats_;
    AStarStats astar_stats_;
    bool race_started_ = false;
    bool race_finished_ = false;

public:
    explicit RaceMode(Maze& maze);

    // Race control
    void start_race();
    void reset_race();
    bool is_race_active() const { return race_started_ && !race_finished_; }
    bool is_race_finished() const { return race_finished_; }

    // Player movement commands
    bool move_up();
    bool move_down();
    bool move_left();
    bool move_right();

    // Get current state
    std::pair<int, int> get_current_position() const { return current_position_; }
    std::pair<int, int> get_entrance() const { return maze_.get_entrance(); }
    std::pair<int, int> get_exit() const { return maze_.get_exit(); }
    int get_player_moves() const { return player_stats_.moves; }
    double get_elapsed_time() const;

    // Execute A* after player finishes
    void run_astar();

    // Results comparison
    void print_comparison() const;
    void save_results_to_file(const std::string& filename) const;

    // Display current maze state with player position
    void print_current_state() const;

private:
    bool is_valid_move(int from_row, int from_col, int to_row, int to_col) const;
    bool try_move(int new_row, int new_col);
    void check_if_finished();
    std::string format_percentage(double value, double reference) const;
};

} // namespace course

#endif // RACEMODE_H