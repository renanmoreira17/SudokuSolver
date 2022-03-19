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

    // goal game
    // std::string game("..9.....43..6..2......53.........81.16.4.5..223..8...9...21....94..36.........6..");
    // simplified goal game
    std::string game("6.9.2...43..6..2......53..6.9.362815168495..2235.8.469..621...394..36.....3...6..");

    // unique rectangles - type 1
    // const auto game("12...794396..1.87247.9..1563.71.9.256.9..5731512.7..987.1...2.48.6...3172347.15.9");
    // unique rectangles - type 2
    // const auto game("...8.6...2...1..74..97...1...6...2.13.....6...2........3...5.....2....8.81...2953");
    // unique rectangles - type 2b without 3d medusa
    // const auto game(".4186539..9..4..6..3.7924.1.28...94.519624..3.7.9.821.15..8.629.6..19.3.98.2.61..");
    // unique rectangles - type 3
    // const auto game(
    // "{69}{69}{128}5{12}347{18}5{73}{12}8{127}4{139}6{39}4{73}{18}{17}96{138}52857{123}{13}96{12}43246{18}"
    // "759{18}{19}{19}6{42}{48}5{28}37285{73}61{379}4{39}{167}{16}9{347}{347}8{1237}{12}5{17}43952{17}86");
    // unique rectangles - type 3b
    // const auto game("419.2...6.6.1.9....3.465921.9.2.1.8...1.5.29..7.9.4.1...65.2.79.5.398.6292......8");
    // unique rectangles - type 4
    // const std::string game(
    // "31.2..645..5.4.1...4..5.9...32....98.5.....36.9..3..51421863579.63.27814...4..362");
    // hidden unique rectangle - type 1 and 2
    // const std::string game(
    //     "1.957.3...7.39..1...3.1.597.8.743...492.5.78373.289.4.317.2.4..26..3..7.95..67231");

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
