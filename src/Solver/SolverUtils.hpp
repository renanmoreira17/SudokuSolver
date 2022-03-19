#ifndef __SOLVERUTILS_H__
#define __SOLVERUTILS_H__

#include "SolverTypes.hpp"
#include "Util/GlobalDefinitions.hpp"

class Tile;
class Region;
class SuggestionsQuantity;
class SolverTile;
class SolverRegion;
class SolverLine;

namespace SolverUtils
{
bool areTilesInTheSameLine(const Tile& tile1, const Tile& tile2, LineOrientation lineOrientation);
bool areTilesInAnySameLine(const Tile& tile1, const Tile& tile2);
bool areTilesInTheSameSubgrid(const Tile& tile1, const Tile& tile2);

bool areTilesInTheSameRegion(const Tile& tile1, const Tile& tile2);

Region* getTilesCommonRegion(const Tile& tile1, const Tile& tile2);

std::vector<SolverRegion*> getSolverTilesCommonSolverRegions(const SolverTilePtr& tile1,
                                                             const SolverTilePtr& tile2);

SolverLine* getCommonSolverLine(const SolverTilePtr& tile1, const SolverTilePtr& tile2);

SolverTileVec getSeenTiles(const SolverTilePtr& tile);
SolverTileVec getMutuallySeenTiles(const SolverTileVec& tiles);

SuggestionsQuantity collectSuggestionInformation(const SolverTileVec& tiles);

template<typename Container, typename Iterator>
static std::vector<Container> createCombination(const int k, Iterator begin, Iterator end)
{
    if (k == 0)
    {
        return std::vector<Container>();
    }
    std::vector<Container> result;
    auto currentEnd = std::prev(end, k - 1);
    for (auto it = begin; it != currentEnd; ++it)
    {
        std::vector<Container> internalResult = createCombination<Container>(k - 1, std::next(it), end);
        if (internalResult.empty())
        {
            result.emplace_back(Container{*it});
        }
        else
        {
            for (const auto& subCombs : internalResult)
            {
                Container currentResult{*it};
                std::copy(
                    subCombs.begin(), subCombs.end(), std::inserter(currentResult, currentResult.end()));
                result.emplace_back(std::move(currentResult));
            }
        }
    }
    return result;
};

template<typename Container>
static std::vector<Container> createCombination(const int k, const Container& container)
{
    return createCombination<Container>(k, container.begin(), container.end());
};

} // namespace SolverUtils

#endif // __SOLVERUTILS_H__