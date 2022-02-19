#ifndef __TILE_H__
#define __TILE_H__

#include "Util/GlobalDefinitions.hpp"

#include <array>
#include <memory>

class Region;
class Grid;
class Line;
class Subgrid;

class Tile
{
  private:
    Grid* m_grid;
    Coordinates m_coordinates;

    TileValueType m_value;

    mutable Line* m_horizontalLine{nullptr};
    mutable Line* m_verticalLine{nullptr};
    mutable Subgrid* m_subgrid{nullptr};

  public:
    Tile(Grid* grid, Coordinates coordinates, const TileValueType value = 0);
    Tile(const Tile& other);
    Tile(const Tile& other, Grid* grid);
    Tile(Tile&& other) = delete;
    Tile& operator=(const Tile& other) = delete;
    Tile& operator=(Tile&& other) = delete;
    virtual ~Tile() = default;

    bool operator==(const TileValueType value) const;
    Tile& operator=(const TileValueType value);

    void setLine(LineOrientation orientation, Line* line);

    void setHorizontalLine(Line* const& hLine) const;
    Line* getHorizontalLine() const;

    void setVerticalLine(Line* const& vLine) const;
    Line* getVerticalLine() const;

    void setSubgrid(Subgrid* const& subgrid) const;
    Subgrid* getSubgrid() const;

    void setGrid(Grid* grid);
    Grid* getGrid() const;

    const std::tuple<Line*, Line*, Subgrid*> getRegionsTuple();
    const std::tuple<const Line*, const Line*, const Subgrid*> getRegionsTuple() const;

    std::array<Region*, 3> getRegions() const;

    const Coordinates& getCoordinates() const;

    virtual void setValue(const TileValueType value);
    TileValueType getValue() const;
    bool hasValue() const;
};

#endif // __TILE_H__
