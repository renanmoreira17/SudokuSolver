#include "Games.hpp"

#include <iostream>
#include <random>

#include <vector>

const static std::vector<std::string> simpleGames = {
    ".7...51.6.6.1....8....8..4.....43...9...614......9..5.5.43...9.7..4..53......8...",
    ".4...5.2.3.7.....6..9..7854...2...3......65......8....4.6...27..3.......2.893....",
    "..86...95..75.........1.8......5.67...94......4.831...8......1...3..6..9...943...",
    "2..71685....9...2....5....1.5.49.67.7..6..2..64.8...........5.........189.....462"};

const static std::vector<std::string> easyGames = {
    "...............9..1...5.....5..6.2........19.7321...645.43....76..2.......95.4.1.",
    "...7.......5.2...6..2..68..6.4...29........4.5.9......45.27.......4.8.71..7.1...5",
    "2......4941.....6..6....51.6....38.4...91......8.5....3562............9..24......",
    "7.5...6.9....5..2.6.8.72..49.15..2......4....2...6....8.6........91..54.....8....",
    "15......72.....1...7.1.9....18.7.3......9..5...4.....28...53....35.47..9.9......."};

const static std::vector<std::string> intermediateGames = {
    ".6829..4.5..81.2.....4..1.782..7..9.4..........91.........2....75.......941.8....",
    ".25.8........5..26..6...1......3...46....89...12.4.6.54..9....2..8......93.......",
    ".3..6..57..7...9...6...1..3..1.976.4.8............4..9.5...6.7....7..14.......3..",
    "...612..7...3.91.2.....7.4.7....6..1..3.9.....2....4591.....7.3.....1..5.9..3....",
    "........6.....2....6.78...9......7.89.5.....1.1.823...5..2..9....6...3....36.5.82"};

const static std::vector<std::string> expertGames = {
    ".6.9...58.3.......8.56..2.79.4.5....3..79......68..5.47....84......7..69......7..",
    ".........5..26..8..28.....9....2.3....54....639.....72..6..9..39..5.6....4.8.....",
    "...8.....6..21....92.7..1.8....7...1.......92.....5.6....3.9.7..75.2.8..29.......",
    ".........51..2..733....69.2.7.16.....5...3..7.369........235........4..67...1..3.",
    "..9.....43..6..2......53.........81.16.4.5..223..8...9...21....94..36.........6.."};

// convert GameDifficulty to string
std::string gameDifficultyToString(GameDifficulty difficulty)
{
    switch (difficulty)
    {
    case GameDifficulty::Simple: return "Simple";
    case GameDifficulty::Easy: return "Easy";
    case GameDifficulty::Intermediate: return "Intermediate";
    case GameDifficulty::Expert: return "Expert";
    default: return "Unknown";
    }
}

const std::string& getGameOfDifficulty(GameDifficulty difficulty)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1000);

    const auto getRandomNumber = [&](const unsigned int upTo) -> unsigned int {
        return dist(rng) % upTo;
    };

    switch (difficulty)
    {
    case GameDifficulty::Simple: return simpleGames[getRandomNumber(simpleGames.size())];
    case GameDifficulty::Easy: return easyGames[getRandomNumber(easyGames.size())];
    case GameDifficulty::Intermediate: return intermediateGames[getRandomNumber(intermediateGames.size())];
    case GameDifficulty::Expert: return expertGames[getRandomNumber(expertGames.size())];
    case GameDifficulty::Any:
        const auto randomDifficulty = static_cast<GameDifficulty>(getRandomNumber(5));
        if (randomDifficulty != GameDifficulty::Any)
        {
            std::cout << "Random difficulty: " << gameDifficultyToString(randomDifficulty) << std::endl;
        }
        return getGameOfDifficulty(randomDifficulty);
    }
}