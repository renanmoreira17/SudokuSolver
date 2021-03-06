#include "SolverUtils.hpp"

#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"
#include "Solver/SolverRegions.hpp"
#include "SolverTile.hpp"
#include "SuggestionsQuantity.hpp"

#include <cassert>

bool SolverUtils::areTilesInTheSameLine(const Tile& tile1, const Tile& tile2, LineOrientation lineOrientation)
{
    if (tile1.getCoordinates().row == tile2.getCoordinates().row)
    {
        return lineOrientation == LineOrientation::HORIZONTAL;
    }
    else if (tile1.getCoordinates().col == tile2.getCoordinates().col)
    {
        return lineOrientation == LineOrientation::VERTICAL;
    }
    else
    {
        return false;
    }
}

bool SolverUtils::areTilesInAnySameLine(const Tile& tile1, const Tile& tile2)
{
    return areTilesInTheSameLine(tile1, tile2, LineOrientation::HORIZONTAL) ||
           areTilesInTheSameLine(tile1, tile2, LineOrientation::VERTICAL);
}

bool SolverUtils::areTilesInTheSameSubgrid(const Tile& tile1, const Tile& tile2)
{
    const auto& tile1Coordinates = tile1.getCoordinates();
    const auto& tile2Coordinates = tile2.getCoordinates();

    const auto tile1SubgridRow = tile1Coordinates.row / 3;
    const auto tile1SubgridCol = tile1Coordinates.col / 3;
    const auto tile2SubgridRow = tile2Coordinates.row / 3;
    const auto tile2SubgridCol = tile2Coordinates.col / 3;

    return tile1SubgridRow == tile2SubgridRow && tile1SubgridCol == tile2SubgridCol;
}

bool SolverUtils::areTilesInTheSameRegion(const Tile& tile1, const Tile& tile2)
{
    return areTilesInTheSameLine(tile1, tile2, LineOrientation::HORIZONTAL) ||
           areTilesInTheSameLine(tile1, tile2, LineOrientation::VERTICAL) ||
           areTilesInTheSameSubgrid(tile1, tile2);
}

SuggestionsQuantity SolverUtils::collectSuggestionInformation(const SolverTileVec& tiles)
{
    SuggestionsQuantity suggestionsQuan;
    for (const auto& tile : tiles)
    {
        const auto& tileSuggestions = tile->getSuggestions();
        for (const auto& suggestion : tileSuggestions) { suggestionsQuan.addSuggestion(suggestion); }
    }
    return suggestionsQuan;
}

Region* SolverUtils::getTilesCommonRegion(const Tile& tile1, const Tile& tile2)
{
    if (!areTilesInTheSameRegion(tile1, tile2))
    {
        return nullptr;
    }
    else if (areTilesInTheSameSubgrid(tile1, tile2))
    {
        return tile1.getSubgrid();
    }
    else if (areTilesInTheSameLine(tile1, tile2, LineOrientation::HORIZONTAL))
    {
        return tile1.getHorizontalLine();
    }
    else if (areTilesInTheSameLine(tile1, tile2, LineOrientation::VERTICAL))
    {
        return tile1.getVerticalLine();
    }
    else
    {
        assert(0);
        return nullptr;
    }
}

std::vector<SolverRegion*> SolverUtils::getSolverTilesCommonSolverRegions(const SolverTilePtr& tile1,
                                                                          const SolverTilePtr& tile2)
{
    std::vector<SolverRegion*> result;
    const auto tile1Regions = tile1->getSolverRegions();
    const auto tile2Regions = tile2->getSolverRegions();
    for (const auto& tile1Region : tile1Regions)
    {
        for (const auto& tile2Region : tile2Regions)
        {
            if (tile1Region == tile2Region)
            {
                result.emplace_back(tile1Region);
            }
        }
    }
    return result;
}

SolverLine* SolverUtils::getCommonSolverLine(const SolverTilePtr& tile1, const SolverTilePtr& tile2)
{
    if (areTilesInTheSameLine(*tile1, *tile2, LineOrientation::HORIZONTAL))
    {
        return tile1->getSolverHorizontalLine();
    }
    else if (areTilesInTheSameLine(*tile1, *tile2, LineOrientation::VERTICAL))
    {
        return tile1->getSolverVerticalLine();
    }
    else
    {
        return nullptr;
    }
}

SolverTileVec SolverUtils::getSeenTiles(const SolverTilePtr& tile)
{
    std::unordered_set<SolverTilePtr> result;
    for (const auto& region : tile->getSolverRegions())
    {
        for (const auto& regionTile : region->getSolverTiles())
        {
            if (regionTile != tile)
            {
                result.emplace(regionTile);
            }
        }
    }
    return {result.begin(), result.end()};
}

SolverTileVec SolverUtils::getMutuallySeenTiles(const SolverTileVec& tiles)
{
    if (tiles.size() < 2)
    {
        return {};
    }
    SolverTileVec result;
    const auto& referenceTile = *tiles.begin();
    result = getSeenTiles(referenceTile);
    for (auto it = tiles.begin() + 1; it != tiles.end(); ++it)
    {
        const auto& currentTile = *it;
        std::erase_if(result, [&](const auto& seenTile) {
            return (seenTile == currentTile) || !areTilesInTheSameRegion(*seenTile, *currentTile);
        });
    }
    return result;
}
