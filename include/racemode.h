//
// Race Mode Header with persistent state support
//

#ifndef RACEMODE_H
#define RACEMODE_H

#include "maze.h"
#include "astar.h"
#include <chrono>
#include <vector>
#include <utility>
#include <string>

namespace course {

    class RaceMode {
    public:
        struct PlayerStats {
            int moves = 0;
            double time_seconds = 0.0;
            bool completed = false;
            std::vector<std::pair<int, int>> path;
        };

        struct AStarStats {
            int moves = 0;
            double time_seconds = 0.0;
            bool completed = false;
            std::vector<std::pair<int, int>> path;
        };

        explicit RaceMode(Maze& maze);

        // Race control
        void start_race();
        void reset_race();
        bool is_race_finished() const { return race_finished_; }

        // Movement
        bool move_up();
        bool move_down();
        bool move_left();
        bool move_right();

        // Display
        void print_current_state() const;
        void print_comparison() const;

        // Results
        void save_results_to_file(const std::string& filename) const;

    private:
        Maze& maze_;
        std::pair<int, int> current_position_;
        bool race_started_ = false;
        bool race_finished_ = false;

        PlayerStats player_stats_;
        AStarStats astar_stats_;

        std::chrono::steady_clock::time_point start_time_;

        // State persistence
        void save_state() const;
        void load_state();

        // Helper methods
        bool is_valid_move(int from_row, int from_col, int to_row, int to_col) const;
        bool try_move(int new_row, int new_col, const std::string& direction);
        void check_if_finished();
        void run_astar();
        double get_elapsed_time() const;
        std::string format_percentage(double value, double reference) const;
    };

} // namespace course

#endif // RACEMODE_H