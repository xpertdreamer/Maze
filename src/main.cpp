#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include "astar.h"
#include "maze.h"
#include "racemode.h"

namespace fs = std::filesystem;

const std::string TEMP_FILE = "maze_temp.txt";
const std::string RACE_RESULTS_FILE = "race_results.txt";

void print_help() {
    std::cout << "Maze Path Finder - Persistent Version with Race Mode\n";
    std::cout << "  maze.exe [command] [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  help                    - Show this help message\n";
    std::cout << "  gen <rows> <cols>       - Generate new maze (auto-saves)\n";
    std::cout << "  load <filename>         - Load maze from file (auto-saves)\n";
    std::cout << "  save [filename]         - Save current maze to file\n";
    std::cout << "  find                    - Find path in current maze (A*)\n";
    std::cout << "  print                   - Print current maze\n";
    std::cout << "  full <rows> <cols> <out> - Generate and save maze\n";
    std::cout << "  current                 - Show current maze status\n\n";
    std::cout << "Race Mode Commands:\n";
    std::cout << "  race_start              - Start race mode\n";
    std::cout << "  race_reset              - Reset current race\n";
    std::cout << "  race_state              - Show current race state\n";
    std::cout << "  race_up                 - Move up\n";
    std::cout << "  race_down               - Move down\n";
    std::cout << "  race_left               - Move left\n";
    std::cout << "  race_right              - Move right\n\n";
    std::cout << "Examples:\n";
    std::cout << "  maze.exe gen 10 15\n";
    std::cout << "  maze.exe race_start\n";
    std::cout << "  maze.exe race_up\n";
}

bool validate_maze_size(int rows, int cols) {
    return rows > 0 && cols > 0 && rows <= 60 && cols <= 60;
}

bool load_current_maze(course::Maze& maze) {
    if (!fs::exists(TEMP_FILE)) {
        return false;
    }

    try {
        maze.from_file(TEMP_FILE);
        return true;
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load temporary maze: " << e.what() << "\n";
        return false;
    }
}

bool save_current_maze(course::Maze& maze, const std::string& filename = TEMP_FILE) {
    try {
        maze.to_file(filename);
        return true;
    } catch (const std::exception& e) {
        std::cout << "Error saving maze: " << e.what() << "\n";
        return false;
    }
}

void print_current_status(const bool maze_loaded, const course::Maze& maze) {
    if (maze_loaded) {
        std::cout << "Current maze: " << maze.getRows() << "x" << maze.getCols() << "\n";
        std::cout << "Entrance: (" << maze.get_entrance().first << ", " << maze.get_entrance().second << ")\n";
        std::cout << "Exit: (" << maze.get_exit().first << ", " << maze.get_exit().second << ")\n";
        std::cout << "Saved in: " << TEMP_FILE << "\n";
    } else {
        std::cout << "No maze loaded. Use 'gen' or 'load' command first.\n";
    }
}

int main(const int argc, char **argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    const std::string command = argv[1];
    bool maze_loaded = false;

    try {
        course::Maze maze;

        // Load maze for commands that need it
        if (command == "find" || command == "print" || command == "save" ||
            command == "current" || command.find("race_") == 0) {
            maze_loaded = load_current_maze(maze);

            if (!maze_loaded && command != "current" && command.find("race_") != 0) {
                std::cout << "Error: No maze loaded. Use 'gen' or 'load' command first.\n";
                return 1;
            }
        }

        // Standard maze commands
        if (command == "help") {
            print_help();
            return 0;
        }
        if (command == "current") {
            print_current_status(maze_loaded, maze);
            return 0;
        }
        if (command == "gen") {
            if (argc != 4) {
                std::cout << "Error: gen requires rows and cols arguments\n";
                return 1;
            }

            int rows = std::stoi(argv[2]);
            int cols = std::stoi(argv[3]);

            if (!validate_maze_size(rows, cols)) {
                std::cout << "Error: Rows and cols must be between 1 and 60\n";
                return 1;
            }

            maze.set_sizes(rows, cols);
            maze.clear_gen();
            maze.generate_maze();

            if (save_current_maze(maze)) {
                std::cout << "SUCCESS: Maze " << rows << "x" << cols << " generated and saved\n";
                maze.print_maze();
                return 0;
            } else {
                return 1;
            }
        }
        if (command == "load") {
            if (argc != 3) {
                std::cout << "Error: load requires filename argument\n";
                return 1;
            }

            std::string filename = argv[2];
            if (!fs::exists(filename)) {
                std::cout << "Error: File '" << filename << "' not found\n";
                return 1;
            }

            maze.from_file(filename);

            if (save_current_maze(maze)) {
                std::cout << "SUCCESS: Maze loaded from '" << filename << "' and saved\n";
                maze.print_maze();
                return 0;
            } else {
                return 1;
            }
        }
        if (command == "save") {
            std::string filename = TEMP_FILE;
            if (argc >= 3) {
                filename = argv[2];
            }

            if (save_current_maze(maze, filename)) {
                std::cout << "SUCCESS: Maze saved to '" << filename << "'\n";
                return 0;
            } else {
                return 1;
            }
        }
        if (command == "find") {
            course::Astar astar(maze);
            auto path = astar.find_path();

            if (!path.empty()) {
                astar.print_path(path);
                return 0;
            } else {
                std::cout << "ERROR: No path found!\n";
                return 1;
            }
        }
        if (command == "print") {
            maze.print_maze();
            return 0;
        }
        if (command == "full") {
            if (argc != 5) {
                std::cout << "Error: full requires rows, cols and output filename\n";
                return 1;
            }

            const int rows = std::stoi(argv[2]);
            const int cols = std::stoi(argv[3]);
            const std::string filename = argv[4];

            if (!validate_maze_size(rows, cols)) {
                std::cout << "Error: Rows and cols must be between 1 and 60\n";
                return 1;
            }

            maze.set_sizes(rows, cols);
            maze.clear_gen();
            maze.generate_maze();

            if (save_current_maze(maze, filename) && save_current_maze(maze)) {
                std::cout << "SUCCESS: Generated " << rows << "x" << cols
                        << " maze and saved to '" << filename << "'\n";
                maze.print_maze();
                return 0;
            }
            return 1;
        }

        // Race Mode Commands
        if (command.find("race_") == 0) {
            if (!maze_loaded) {
                std::cout << "ERROR: No maze loaded for race mode!\n";
                std::cout << "Please generate or load a maze first:\n";
                std::cout << "  maze.exe gen 10 15\n";
                return 1;
            }

            // Create race mode instance (stateless, reads from temp file each time)
            course::RaceMode race(maze);

            if (command == "race_start") {
                race.start_race();
                // Save race state indicator
                std::ofstream state_file("race_active.tmp");
                state_file << "1";
                state_file.close();
                return 0;
            }

            // Check if race is active (for movement commands)
            bool race_active = fs::exists("race_active.tmp");

            if (command == "race_reset") {
                if (fs::exists("race_active.tmp")) {
                    fs::remove("race_active.tmp");
                }
                race.reset_race();
                return 0;
            }

            if (command == "race_state") {
                if (race_active) {
                    race.print_current_state();
                } else {
                    std::cout << "No active race. Use 'race_start' to begin.\n";
                }
                return 0;
            }

            // Movement commands require active race
            if (!race_active) {
                std::cout << "ERROR: No active race! Use 'race_start' first.\n";
                return 1;
            }

            // Start race automatically for movement commands
            race.start_race();

            bool moved = false;
            if (command == "race_up") {
                moved = race.move_up();
            } else if (command == "race_down") {
                moved = race.move_down();
            } else if (command == "race_left") {
                moved = race.move_left();
            } else if (command == "race_right") {
                moved = race.move_right();
            } else {
                std::cout << "Unknown race command: " << command << "\n";
                return 1;
            }

            if (moved) {
                race.print_current_state();
            }

            // Check if race finished
            if (race.is_race_finished()) {
                if (fs::exists("race_active.tmp")) {
                    fs::remove("race_active.tmp");
                }
                race.save_results_to_file(RACE_RESULTS_FILE);
            }

            return 0;
        }

        std::cout << "Error: Unknown command '" << command << "'\n";
        print_help();
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
        return 1;
    }
}