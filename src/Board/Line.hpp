#ifndef __LINE_H__
#define __LINE_H__

#include "Region.hpp"

class Grid;
class Tile;

class Line: public Region<Tile>
{
private:
    Orientation m_orientation;
    Grid* m_grid;

public:
    Line(Grid* grid, Orientation orientation, const TileValueType index);
    Line()                              = default;
    Line(const Line& other)             = default;
    Line(Line&& other);
    Line& operator=(const Line& other) = default;
    Line& operator=(Line&& other);
    ~Line() {}

    const Orientation& getOrientation() const { return m_orientation; }
};

#endif // __LINE_H__
