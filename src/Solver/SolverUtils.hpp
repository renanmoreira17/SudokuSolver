#ifndef __SOLVERUTILS_H__
#define __SOLVERUTILS_H__

#include "GlobalDefinitions.hpp"

#include <memory>

class Tile;

namespace SolverUtils
{
bool areTilesInTheSameLine(const Tile& tile1, const Tile& tile2, LineOrientation lineOrientation);
bool areTilesInTheSameSubgrid(const Tile& tile1, const Tile& tile2);
} // namespace SolverUtils

#endif // __SOLVERUTILS_H__