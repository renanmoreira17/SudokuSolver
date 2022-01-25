#ifndef __GLOBALDEFINITIONS_H__
#define __GLOBALDEFINITIONS_H__

using TileValueType = unsigned short;

enum class LineOrientation
{
    VERTICAL,
    HORIZONTAL
};

enum class RegionSpecificType
{
    UNKNOWN,
    HORIZONTAL_LINE,
    VERTICAL_LINE,
    SUBGRID
};

#endif // __GLOBALDEFINITIONS_H__