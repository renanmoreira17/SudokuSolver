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
    // std::string game(
    //     "5...8..49...5...3..673....115..........2.8..........187....415..3...2...49..5...3");

    const auto game = getGameOfDifficulty(GameDifficulty::Expert);

    // std::string game("...612..7...3.91.2.....7.4.7....6..1..3.9.....2....4591.....7.3.....1..5.9..3....");
    // const std::string game("000000070000090810500203004800020000045000720000000003400308006072010000030000000");
    // const std::string game("..9.....43..6..2......53.........81.16.4.5..223..8...9...21....94..36.........6..");
    // const std::string game("..7.836...397.68..82641975364.19.387.8.367....73.48.6.39.87..267649..1382.863.97."); //simple coloring

    std::cout << "Game: \n" << game << std::endl << std::endl;

        Solver solver = Solver(game);
    std::shared_ptr<Reporter> reporter = std::make_shared<Reporter>(printAction);
    solver.setReporter(reporter);
    solver.solve();
    return 0;
}
