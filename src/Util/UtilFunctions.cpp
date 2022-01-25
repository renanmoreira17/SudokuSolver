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
