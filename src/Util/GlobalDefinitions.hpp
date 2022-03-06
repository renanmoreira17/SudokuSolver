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

struct Coordinates
{
    TileValueType row;
    TileValueType col;

    bool operator==(const Coordinates& other) const;
};

#endif // __GLOBALDEFINITIONS_H__