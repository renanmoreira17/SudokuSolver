add_executable(SudokuSolver main.cpp)

add_library(GlobalDefinitions INTERFACE GlobalDefinitions.hpp)
target_include_directories(GlobalDefinitions INTERFACE .)

add_subdirectory(Board)
add_subdirectory(Solver)
add_subdirectory(Util)

target_link_libraries(${PROJECT_NAME} Board Solver)

# add_definitions(DEBUG)
