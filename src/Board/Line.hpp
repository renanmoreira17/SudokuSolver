#ifndef __LINE_H__
#define __LINE_H__

#include "Region.hpp"

class Grid;
class Tile;

class Line : virtual public Region
{
  private:
    LineOrientation m_orientation;

  public:
    Line(Grid* grid, LineOrientation orientation, const short index);
    Line(const Line& other);
    Line(Line&& other);
    Line& operator=(const Line& other);
    Line& operator=(Line&& other);
    virtual ~Line() = default;

    const LineOrientation& getLineOrientation() const { return m_orientation; }

    RegionSpecificType getRegionSpecificType() const override
    {
        switch (getLineOrientation())
        {
        case LineOrientation::HORIZONTAL: return RegionSpecificType::HORIZONTAL_LINE;
        case LineOrientation::VERTICAL: return RegionSpecificType::VERTICAL_LINE;
        }
    }
};

#endif // __LINE_H__
