#ifndef __SOLVERUTILS_H__
#define __SOLVERUTILS_H__

#include "GlobalDefinitions.hpp"

#include <memory>

class Tile;

bool areTilesInTheSameLine(const Tile& tile1, const Tile& tile2, LineOrientation lineOrientation);
bool areTilesInTheSameSubgrid(const Tile& tile1, const Tile& tile2);

#endif // __SOLVERUTILS_H__