include_directories(.)

add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Og" "-g3")

add_subdirectory(Util)
add_subdirectory(Board)
add_subdirectory(Solver)
add_subdirectory(GamesManager)

add_executable(SudokuSolver main.cpp)


target_link_libraries(${PROJECT_NAME} Board Solver Games)

# add_definitions(DEBUG)
