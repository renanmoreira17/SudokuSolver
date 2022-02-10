#ifndef __SOLVERTYPES_H__
#define __SOLVERTYPES_H__

class SolverTile;

#include <memory>
#include <vector>

using SolverTilePtr = std::shared_ptr<SolverTile>;
using SolverTileVec = std::vector<SolverTilePtr>;

#endif // __SOLVERTYPES_H__