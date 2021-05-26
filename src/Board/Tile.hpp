#ifndef __TILE_H__
#define __TILE_H__

#include <array>
#include "GlobalDefinitions.hpp"

struct Coordinates
{
    TileValueType row;
    TileValueType col;
};

template <typename T>
class Region;

class Grid;
class Line;
class Subgrid;

class Tile
{
private:
    Grid *m_grid;
    Coordinates m_coordinates;

    mutable Line *m_hLine;
    mutable Line *m_vLine;
    mutable Subgrid *m_subgrid;

    std::array<Region<Tile> **, 3> m_regions;

    TileValueType m_value;

public:
    Tile(Grid *grid, Coordinates coordinates, const TileValueType value = 0);
    Tile() = default;
    Tile(const Tile &other);
    Tile(Tile &&other);
    Tile &operator=(const Tile &other);
    Tile &operator=(Tile &&other);
    ~Tile() = default;

    bool operator==(const TileValueType value) const;
    Tile &operator=(const TileValueType value);

    void setHorizontalLine(Line *const &hLine) const;
    Line &getHorizontalLine();
    const Line &getHorizontalLine() const;
    void setVerticalLine(Line *const &vLine) const;
    Line &getVerticalLine();
    const Line &getVerticalLine() const;
    void setSubgrid(Subgrid *const &subgrid) const;
    Subgrid &getSubgrid();
    const Subgrid &getSubgrid() const;
    const std::tuple<Line &, Line &, Subgrid &> getRegionsTuple();
    const std::tuple<const Line &, const Line &, const Subgrid &> getRegionsTuple() const;

    const std::array<Region<Tile> **, 3> &getRegions() const;

    const Coordinates &getCoordinates() const;

    void setValue(const TileValueType value);
    TileValueType getValue() const;
    bool hasValue() const;
};

#endif // __TILE_H__
