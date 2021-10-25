#ifndef __LINE_H__
#define __LINE_H__

#include "Region.hpp"

class Grid;
class Tile;

class Line : virtual public Region
{
  private:
    LineOrientation m_orientation;
    Grid* m_grid;

  public:
    Line(Grid* grid, LineOrientation orientation, const short index);
    Line(const Line& other) = default;
    Line(Line&& other);
    Line& operator=(const Line& other) = default;
    Line& operator=(Line&& other);
    virtual ~Line() = default;

    const LineOrientation& getLineOrientation() const { return m_orientation; }
};

#endif // __LINE_H__
