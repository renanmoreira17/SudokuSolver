#include "UtilFunctions.hpp"

char convertRowToLetter(const TileValueType row)
{
    return static_cast<char>(row + 'A');
}

std::string convertRegionSpecificTypeToString(RegionSpecificType regionSpecificType)
{
    switch (regionSpecificType)
    {
    case RegionSpecificType::SUBGRID: return "SUBGRID";
    case RegionSpecificType::HORIZONTAL_LINE: return "HORIZONTAL_LINE";
    case RegionSpecificType::VERTICAL_LINE: return "VERTICAL_LINE";
    default: return "UNKNOWN";
    }
}

Coordinates transformRelativeSubgridToAbsoluteCoordinates(const Coordinates& relativeCoordinates,
                                                          TileValueType subgridIndex)
{
    const TileValueType offsetCol = subgridIndex % 3;
    const TileValueType offsetRow = subgridIndex / 3;

    const TileValueType col = offsetCol * 3 + relativeCoordinates.col;
    const TileValueType row = offsetRow * 3 + relativeCoordinates.row;

    return {row, col};
}
Coordinates transformSubgridIndexedToAbsoluteCoordinates(const TileValueType indexedCoordinate,
                                                         const TileValueType subgridIndex)
{
    const TileValueType offsetCol = subgridIndex % 3;
    const TileValueType offsetRow = subgridIndex / 3;

    const TileValueType col = offsetCol * 3 + indexedCoordinate % 3;
    const TileValueType row = offsetRow * 3 + indexedCoordinate / 3;

    return {row, col};
}
