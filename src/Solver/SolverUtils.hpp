#ifndef __SOLVERUTILS_H__
#define __SOLVERUTILS_H__

#include "GlobalDefinitions.hpp"

#include <memory>
#include <vector>

class Tile;
class Region;
class SuggestionsQuantity;
class SolverTile;

namespace SolverUtils
{
bool areTilesInTheSameLine(const Tile& tile1, const Tile& tile2, LineOrientation lineOrientation);
bool areTilesInTheSameSubgrid(const Tile& tile1, const Tile& tile2);

bool areTilesInTheSameRegion(const Tile& tile1, const Tile& tile2);

Region* getTilesCommonRegion(const Tile& tile1, const Tile& tile2);

SuggestionsQuantity
collectSuggestionInformation(const std::vector<std::shared_ptr<SolverTile>>& tiles);
} // namespace SolverUtils

#endif // __SOLVERUTILS_H__