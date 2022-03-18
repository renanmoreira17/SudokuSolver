#include "UniqueRectanglesCreator.hpp"

#include "Solver/SolverRegions.hpp"
#include "Solver/SolverTile.hpp"
#include "Solver/SolverTypes.hpp"
#include "Solver/SolverUtils.hpp"

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

UniqueRectanglesCreator::UniqueRectanglesCreator(std::vector<TileValueType> biValueSuggestions,
                                                 std::shared_ptr<SolverLine> vLine)
    : m_biValueSuggestions(std::move(biValueSuggestions))
    , m_vLine(std::move(vLine))
{}

std::vector<SolverTileVec> UniqueRectanglesCreator::buildPossibleRectangles() const
{
    const auto tiles = m_vLine->getTilesWithAllSuggestions(m_biValueSuggestions);
    if (tiles.size() < 2)
    {
        return {};
    }

    std::vector<SolverTileVec> possibleRectangleTiles;
    const auto tileCombs = SolverUtils::createCombination(2, tiles);

    for (const auto& tileComb : tileCombs)
    {
        // we must check if the tiles we got form a rectangle. for that, we must check the horizontal
        // lines of the tiles, and check if they form a pair, and if the pairs are on the same
        // vertical line.

        const auto& startingTile = tileComb.at(0);
        const auto& finishingTile = tileComb.at(1);

        const auto pairsForStarting =
            getPossiblePairs(startingTile->getSolverHorizontalLine(), startingTile, m_biValueSuggestions);
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
                                 m_biValueSuggestions);
            if (possibleSecondPairs.empty())
            {
                continue;
            }

            // pair for second edge
            for (const auto& possibleSecondPair : possibleSecondPairs)
            {
                const auto possibleThirdPairs = getPossiblePairs(
                    possibleSecondPair->getSolverHorizontalLine(), possibleSecondPair, m_biValueSuggestions);
                if (possibleThirdPairs.empty())
                {
                    continue;
                }

                // pair for third edge (hopefully the finishing tile)
                for (const auto& possibleThirdPair : possibleThirdPairs)
                {
                    if (possibleThirdPair == finishingTile)
                    {
                        SolverTileVec possible = {
                            startingTile, possiblePairForStarting, possibleSecondPair, possibleThirdPair};
                        // we must check if all those tiles are contained in only 2 subgrids.
                        std::unordered_set<const SolverSubgrid*> subgrids;
                        for (const auto& tile : possible) { subgrids.insert(tile->getSolverSubgrid()); }
                        if (subgrids.size() != 2)
                        {
                            continue;
                        }
                        possibleRectangleTiles.emplace_back(std::move(possible));
                        // there can't be more rectangles
                        break;
                    }
                }
            }
        }
    }

    return possibleRectangleTiles;
}
