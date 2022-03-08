#include "UniqueRectangles.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"
#include "Solver/SolverUtils.hpp"
#include "Tools/Chain.hpp"
#include "Tools/Traversal.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <optional>
#include <unordered_set>

#include "Util/UtilFunctions.hpp"
#include <fmt/format.h>
#include <iostream>

namespace
{
class UniqueRectanglesAnalyzer
{
  public:
    UniqueRectanglesAnalyzer(const SolverTileVec& tiles,
                             const std::vector<TileValueType>& suggestions,
                             const Solver& solver)
        : m_solver(solver)
        , m_targetSuggestions(suggestions)
        , m_tiles(tiles)
    {
        assert(tiles.size() == 4);
        for (const auto& tile : tiles)
        {
            if (tile->getSuggestionsCount() == 2)
            {
                m_floorTiles.emplace_back(tile);
            }
            else
            {
                m_roofTiles.emplace_back(tile);
            }
        }
    }

    bool analyzeAndPerform() const
    {
        if (analyzeType1())
        {
            return true;
        }
        return false;
    }

  private:
    bool analyzeType1() const
    {
        if (m_roofTiles.size() != 1)
        {
            return false;
        }
        const auto& roofTile = m_roofTiles.front();
        roofTile->removeSuggestions(m_targetSuggestions);
        m_solver.report("Unique Rectangles - Type 1:\nOs tiles {} formam um Unique Rectangle, com as "
                        "sugestões {}. No entanto, o tile {} possui sugestões extras além dessas. Assim, as "
                        "sugestões {} foram removidas do Tile {} para que o Sudoku tenha uma solução única.",
                        fmt::join(m_tiles, ", "),
                        fmt::join(m_targetSuggestions, ", "),
                        roofTile,
                        fmt::join(m_targetSuggestions, ", "),
                        roofTile);
        return true;
    }

    const Solver& m_solver;
    std::vector<TileValueType> m_targetSuggestions;
    const SolverTileVec m_tiles;
    SolverTileVec m_roofTiles;
    SolverTileVec m_floorTiles;
};
} // namespace

bool UniqueRectangles::analyze()
{
    std::unordered_map<TileValueType, int> suggestionsPerValue;
    for (const auto& vLine : m_solver.getAllSolverVerticalLines())
    {
        const auto& suggestionsQuan = vLine->getSuggestionsQuan();
        const auto& candidateSuggestions = suggestionsQuan.getSuggestionsWithQuantityGreaterThan(1);
        if (candidateSuggestions.size() >= 2)
        {
            return true;
        }
    }
    return false;
}

namespace
{
SolverTileVec getPossiblePairs(const SolverRegion* fromRegion,
                               const SolverTilePtr& fromTile,
                               const std::vector<TileValueType>& suggestions)
{
    auto tilesWithSuggestions = fromRegion->getTilesWithAllSuggestions(suggestions);
    if (tilesWithSuggestions.size() < 2)
    {
        return {};
    }
    std::erase_if(tilesWithSuggestions, [&fromTile](const SolverTilePtr& t) { return t == fromTile; });
    return tilesWithSuggestions;
}
} // namespace

bool UniqueRectangles::perform()
{
    for (const auto& vLine : m_solver.getAllSolverVerticalLines())
    {
        const auto& sugQuan = vLine->getSuggestionsQuan();
        const auto candidateSuggestions = sugQuan.getSuggestionsWithQuantityGreaterThan(1);
        if (candidateSuggestions.size() < 2)
        {
            continue;
        }

        const auto combinations = SolverUtils::createCombination(2, candidateSuggestions);
        for (const auto& combination : combinations)
        {
            const auto tiles = vLine->getTilesWithAllSuggestions(combination);
            if (tiles.size() < 2)
            {
                continue;
            }

            const auto tileCombs = SolverUtils::createCombination(2, tiles);

            // first two tiles
            for (const auto& tileComb : tileCombs)
            {
                // we must check if the tiles we got form a rectangle. for that, we must check the horizontal
                // lines of the tiles, and check if they form a pair, and if the pairs are on the same
                // vertical line.

                std::vector<SolverTileVec> possibleRectangleTiles;

                const auto& startingTile = tileComb.at(0);
                const auto& finishingTile = tileComb.at(1);

                const auto pairsForStarting =
                    getPossiblePairs(startingTile->getSolverHorizontalLine(), startingTile, combination);
                if (pairsForStarting.empty())
                {
                    continue;
                }

                // pair for first edge
                for (const auto& possiblePairForStarting : pairsForStarting)
                {
                    const auto possibleSecondPairs =
                        getPossiblePairs(possiblePairForStarting->getSolverVerticalLine(),
                                         possiblePairForStarting,
                                         combination);
                    if (possibleSecondPairs.empty())
                    {
                        continue;
                    }

                    // pair for second edge
                    for (const auto& possibleSecondPair : possibleSecondPairs)
                    {
                        const auto possibleThirdPairs = getPossiblePairs(
                            possibleSecondPair->getSolverHorizontalLine(), possibleSecondPair, combination);
                        if (possibleThirdPairs.empty())
                        {
                            continue;
                        }

                        // pair for third edge (hopefully the finishing tile)
                        for (const auto& possibleThirdPair : possibleThirdPairs)
                        {
                            if (possibleThirdPair == finishingTile)
                            {
                                possibleRectangleTiles.push_back({startingTile,
                                                                  possiblePairForStarting,
                                                                  possibleSecondPair,
                                                                  possibleThirdPair});
                                break;
                            }
                        }
                    }
                }

                // if it didn't find any rectangle, must continue to the next iteration
                if (possibleRectangleTiles.empty())
                {
                    continue;
                }

                for (const auto& possibleRectangle : possibleRectangleTiles)
                {
                    // we must check if all those tiles are contained in only 2 subgrids.
                    std::unordered_set<const SolverSubgrid*> subgrids;
                    for (const auto& tile : possibleRectangle) { subgrids.insert(tile->getSolverSubgrid()); }
                    if (subgrids.size() != 2)
                    {
                        continue;
                    }

                    // we must also check if there are at least 2 tiles with 2 suggestions to constitute the
                    // floor tiles

                    unsigned short quanWith2Suggestions = 0;
                    for (const auto& tile : possibleRectangle)
                    {
                        if (tile->getSuggestionsCount() == 2)
                        {
                            ++quanWith2Suggestions;
                        }
                    }
                    if (quanWith2Suggestions < 2)
                    {
                        continue;
                    }

                    const UniqueRectanglesAnalyzer analyzer(possibleRectangle, combination, m_solver);
                    if (analyzer.analyzeAndPerform())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
