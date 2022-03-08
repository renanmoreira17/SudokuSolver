#include "Board/Grid.hpp"
#include "Games.hpp"
#include "Solver/Reporter.hpp"
#include "Solver/Solver.hpp"

#include <iostream>
#include <string>

void printAction(const std::string& action)
{
    std::cout << action << std::endl;
}

int main()
{
    // https://www.7sudoku.com/play-online
    // std::string game("5...8..49...5...3..673....115..........2.8..........187....415..3...2...49..5...3");

    // const auto game = getGameOfDifficulty(GameDifficulty::Expert);

    // std::string game("...612..7...3.91.2.....7.4.7....6..1..3.9.....2....4591.....7.3.....1..5.9..3....");
    // const std::string game(
    //     "000000070000090810500203004800020000045000720000000003400308006072010000030000000");
    // std::string game("..9.....43..6..2......53.........81.16.4.5..223..8...9...21....94..36.........6..");
    // unique rectangles - type 1
    // const auto game("12...794396..1.87247.9..1563.71.9.256.9..5731512.7..987.1...2.48.6...3172347.15.9");
    // unique rectangles - type 2
    // const auto game("...8.6...2...1..74..97...1...6...2.13.....6...2........3...5.....2....8.81...2953");
    // unique rectangles - type 2b without 3d medusa
    const auto game(".4186539..9..4..6..3.7924.1.28...94.519624..3.7.9.821.15..8.629.6..19.3.98.2.61..");
    // const std::string
    // game("..7.836...397.68..82641975364.19.387.8.367....73.48.6.39.87..267649..1382.863.97."); //simple
    // coloring

    std::cout << "Game: \n" << game << std::endl << std::endl;

    Solver solver = Solver(game);
    std::shared_ptr<Reporter> reporter = std::make_shared<Reporter>(printAction);
    solver.setReporter(reporter);
    solver.solve();
    return 0;
}
