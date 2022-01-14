#include "SolverUtils.hpp"

#include "Board/Tile.hpp"

bool SolverUtils::areTilesInTheSameLine(const Tile& tile1,
                                        const Tile& tile2,
                                        LineOrientation lineOrientation)
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
