#include "Games.hpp"
#include "Solver/Reporter.hpp"
#include "Solver/Solver.hpp"

#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

void printAction(const std::string& action)
{
    std::cout << action << std::endl;
}

namespace
{
std::string toLower(std::string_view value)
{
    std::string result(value);
    for (auto& ch : result)
    {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

std::optional<GameDifficulty> parseDifficulty(std::string_view value)
{
    const auto lowerValue = toLower(value);
    if (lowerValue == "any")
        return GameDifficulty::Any;
    if (lowerValue == "simple")
        return GameDifficulty::Simple;
    if (lowerValue == "easy")
        return GameDifficulty::Easy;
    if (lowerValue == "intermediate")
        return GameDifficulty::Intermediate;
    if (lowerValue == "expert")
        return GameDifficulty::Expert;
    return std::nullopt;
}

void printUsage(const char* exe)
{
    std::cout << "Usage:\n"
              << "  " << exe << " [--game <puzzle>] [--difficulty <level>] [--report]\n\n"
              << "Options:\n"
              << "  --game <puzzle>       81-char puzzle string with digits, '.' or '0'\n"
              << "  --difficulty <level> any|simple|easy|intermediate|expert (default: any)\n"
              << "  --report              print technique explanations as they run\n"
              << "  -h, --help            show this help\n";
}
} // namespace

int main(int argc, char** argv)
{
    std::optional<std::string> gameArg;
    std::optional<GameDifficulty> difficultyArg;
    bool report = false;
    bool hasDifficulty = false;

    for (int i = 1; i < argc; ++i)
    {
        const std::string_view arg(argv[i]);
        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        if (arg == "--game")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Missing value for --game\n";
                return 1;
            }
            gameArg = std::string(argv[++i]);
            continue;
        }
        if (arg == "--difficulty")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Missing value for --difficulty\n";
                return 1;
            }
            hasDifficulty = true;
            const auto parsed = parseDifficulty(argv[++i]);
            if (!parsed.has_value())
            {
                std::cerr << "Invalid difficulty value\n";
                return 1;
            }
            difficultyArg = parsed;
            continue;
        }
        if (arg == "--report")
        {
            report = true;
            continue;
        }
        if (!arg.empty() && arg[0] == '-')
        {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
        if (!gameArg.has_value())
        {
            gameArg = std::string(arg);
        }
        else
        {
            std::cerr << "Unexpected argument: " << arg << "\n";
            return 1;
        }
    }

    if (gameArg.has_value() && hasDifficulty)
    {
        std::cerr << "Warning: --game provided, ignoring --difficulty\n";
    }

    const std::string game = gameArg.value_or(getGameOfDifficulty(
        difficultyArg.value_or(GameDifficulty::Any)));
    std::cout << "Game: \n" << game << std::endl << std::endl;

    Solver solver = Solver(game);
    if (report)
    {
        std::shared_ptr<Reporter> reporter = std::make_shared<Reporter>(printAction);
        solver.setReporter(reporter);
    }
    solver.solve();
    return 0;
}
