#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include "astar.h"
#include "maze.h"

namespace fs = std::filesystem;

const std::string TEMP_FILE = "maze_temp.txt";

void print_help() {
    std::cout << "Maze Path Finder - Persistent Version\n";
    std::cout << "  maze.exe [command] [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  help                    - Show this help message\n";
    std::cout << "  gen <rows> <cols>       - Generate new maze (auto-saves)\n";
    std::cout << "  load <filename>         - Load maze from file (auto-saves)\n";
    std::cout << "  save [filename]         - Save current maze to file (default: maze_temp.txt)\n";
    std::cout << "  find                    - Find path in current maze\n";
    std::cout << "  print                   - Print current maze\n";
    std::cout << "  full <rows> <cols> <output> - Generate and save maze\n";
    std::cout << "  current                 - Show current maze status\n\n";
    std::cout << "Examples:\n";
    std::cout << "  maze.exe gen 10 15\n";
    std::cout << "  maze.exe load mymaze.txt\n";
    std::cout << "  maze.exe find\n";
    std::cout << "  maze.exe full 20 25 output.txt\n";
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

        // Try to load existing maze from temp file for commands that need it
        if (command == "find" || command == "print" || command == "save" || command == "current") {
            maze_loaded = load_current_maze(maze);

            if (!maze_loaded && command != "current") {
                std::cout << "Error: No maze loaded. Use 'gen' or 'load' command first.\n";
                return 1;
            }
        }

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

            // Auto-save generated maze
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

            // Auto-save loaded maze
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

            // Save to both specified file and temp file
            if (save_current_maze(maze, filename) && save_current_maze(maze)) {
                std::cout << "SUCCESS: Generated " << rows << "x" << cols
                        << " maze and saved to '" << filename << "'\n";
                maze.print_maze();
                return 0;
            }
            return 1;
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