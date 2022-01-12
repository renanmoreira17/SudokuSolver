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

    const auto game = getGameOfDifficulty(GameDifficulty::Intermediate);

    std::cout << "Game: \n" << game << std::endl << std::endl;

        Solver solver = Solver(game);
    std::shared_ptr<Reporter> reporter = std::make_shared<Reporter>(printAction);
    solver.setReporter(reporter);
    solver.solve();
    return 0;
}
