cmake_minimum_required(VERSION 3.31)
project(Maze VERSION 0.0.1 LANGUAGES CXX)

if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(src)

add_executable(Maze src/main.cpp)

target_link_libraries(Maze PRIVATE
    maze_lib
)

target_include_directories(Maze PRIVATE ${CMAKE_SOURCE_DIR}/include)

set_target_properties(Maze PROPERTIES
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "maze"
)


message(STATUS "Project configured successfully: ${PROJECT_NAME}")
message(STATUS "Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
