cmake_minimum_required(VERSION 3.16)
project(power_grid_algorithms LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GRID_SOURCES
    src/Graph.cpp
    src/Connectivity.cpp
    src/Dijkstra.cpp
    src/UnionFind.cpp
    src/Kruskal.cpp
    src/Scheduler.cpp
)

add_library(power_grid_algorithms STATIC ${GRID_SOURCES})
target_include_directories(power_grid_algorithms PUBLIC include)

add_executable(power_grid_tests tests.cpp)
target_link_libraries(power_grid_tests PRIVATE power_grid_algorithms)
enable_testing()
add_test(NAME power_grid_tests COMMAND power_grid_tests)
