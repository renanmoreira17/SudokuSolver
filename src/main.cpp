#include <iostream>
#include <string>
#include "Board/Grid.hpp"
#include "Solver/Solver.hpp"

int main() {
    // https://www.7sudoku.com/play-online
    std::string game("5...8..49...5...3..673....115..........2.8..........187....415..3...2...49..5...3");

    Solver solver = Solver(game);
    solver.solve();
    return 0;
}
