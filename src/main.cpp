#include <iostream>
#include <sstream>

#include "astar.h"
#include "maze.h"

int main() {
    auto maze_initialized = false;
    course::Maze maze;
    std::string input;

    std::cout << "Commands: " << std::endl;
    std::cout << "gen <rows> <cols>" << std::endl;
    std::cout << "find" << std::endl;
    std::cout << "file <filename>" << std::endl;
    std::cout << "save <filename>" << std::endl;
    std::cout << "print" << std::endl;
    std::cout << "exit" << std::endl;
    while (true) {
        std::cout << "maze> ";
        std::getline(std::cin, input);

        if (input == "exit" || input == "quit") {
            std::cout << "Bye!\n";
            break;
        }

        std::istringstream iss(input);
        std::string command;
        iss >> command;

        if (command == "gen") {
            int rows, cols;
            if (iss >> rows >> cols) {
                if (rows > 0 && cols > 0 && rows <= 60 && cols <= 60) {
                    maze.set_sizes(rows, cols);
                    maze.clear_gen();
                    maze.generate_maze();
                    maze_initialized = true;
                    std::cout << "Maze " << rows << "x" << cols << " generated successfully!" << std::endl;
                    maze.print_maze();
                } else {
                    std::cout << "Error: Rows and cols must be between 1 and 60" << std::endl;
                }
            } else {
                std::cout << "Error: Need two numbers (rows cols)" << std::endl;
            }
        } else if (command == "find") {
            if (!maze_initialized) {
                std::cout << "Error: Maze is not initialized!" << std::endl;
                continue;
            }
            course::Astar astar(maze);
                if (auto path = astar.find_path(); !path.empty()) {
                    #ifdef _WIN32
                    system("cls");
                    #else
                    system("clear");
                    #endif
                    astar.print_path(path);
                } else
                    std::cout << "Path is empty!" << std::endl;
        } else if (command == "file") {
            std::string filename;
            if (iss >> filename) {
                try {
                    maze.from_file(filename);
                    maze_initialized = true;
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            } else {
                std::cout << "Error: need a filename" << std::endl;
            }
        } else if (command == "save") {
            std::string filename;
            if (iss >> filename) {
                if (maze_initialized) {
                    maze.to_file(filename);
                } else {
                    std::cout << "Error: No maze to save. Generate or load one first." << std::endl;
                }
            } else {
                std::cout << "Error: Need filename" << std::endl;
            }
        } else if (command == "print") {
            if (maze_initialized) {
                maze.print_maze();
            } else {
                std::cout << "Error: No maze to print. Generate or load one first." << std::endl;
            }
        } else if (!command.empty()) {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    return 0;
}
