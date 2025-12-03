add_library(maze_lib
        maze_tools/maze.cpp
        matrix.cpp
)

target_include_directories(maze_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        maze_tools
)

target_compile_options(maze_lib PRIVATE ${PROJECT_COMPILE_OPTIONS})
target_link_options(maze_lib PRIVATE ${PROJECT_LINK_OPTIONS})