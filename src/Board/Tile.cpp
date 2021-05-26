#include "Tile.hpp"
#include "Line.hpp"
#include "Subgrid.hpp"
#include "Region.hpp"
#include <utility>

const Coordinates &Tile::getCoordinates() const
{
    return m_coordinates;
}

void Tile::setValue(const TileValueType value)
{
    m_value = value;
}

bool Tile::hasValue() const
{
    return m_value != 0;
}

void Tile::setHorizontalLine(Line *const &hLine) const
{
    m_hLine = hLine;
}
Line &Tile::getHorizontalLine()
{
    return *m_hLine;
}

const Line &Tile::getHorizontalLine() const
{
    return *m_hLine;
}

void Tile::setVerticalLine(Line *const &vLine) const
{
    m_vLine = vLine;
}

Line &Tile::getVerticalLine()
{
    return *m_vLine;
}

const Line &Tile::getVerticalLine() const
{
    return *m_vLine;
}

void Tile::setSubgrid(Subgrid *const &subgrid) const
{
    m_subgrid = subgrid;
}

Subgrid &Tile::getSubgrid()
{
    return *m_subgrid;
}

const Subgrid &Tile::getSubgrid() const
{
    return *m_subgrid;
}

const std::tuple<Line &, Line &, Subgrid &> Tile::getRegionsTuple()
{
    return std::tie(getHorizontalLine(), getVerticalLine(), getSubgrid());
}

const std::tuple<const Line &, const Line &, const Subgrid &> Tile::getRegionsTuple() const
{
    return std::tie(getHorizontalLine(), getVerticalLine(), getSubgrid());
}

const std::array<Region<Tile> **, 3> &Tile::getRegions() const
{
    return m_regions;
}

bool Tile::operator==(const TileValueType value) const
{
    return m_value == value;
}

Tile &Tile::operator=(const TileValueType value)
{
    setValue(value);
    return *this;
}

Tile::Tile(Grid *grid, Coordinates coordinates, const TileValueType value)
    : m_grid(grid), m_coordinates(coordinates), m_regions({(Region<Tile> **)&m_hLine, (Region<Tile> **)&m_vLine, (Region<Tile> **)&m_subgrid}), m_value(value)
{
}

Tile::Tile(const Tile &other)
{
    m_grid = other.m_grid;
    m_coordinates = other.m_coordinates;
    m_value = other.m_value;
    m_hLine = other.m_hLine;
    m_vLine = other.m_vLine;
    m_subgrid = other.m_subgrid;
    m_regions = {(Region<Tile> **)&m_hLine, (Region<Tile> **)&m_vLine, (Region<Tile> **)&m_subgrid};
}

Tile::Tile(Tile &&other)
{
    m_grid = std::move(other.m_grid);
    m_coordinates = std::move(other.m_coordinates);
    m_value = std::move(other.m_value);
    m_hLine = std::move(other.m_hLine);
    m_vLine = std::move(other.m_vLine);
    m_subgrid = std::move(other.m_subgrid);
    m_regions = {(Region<Tile> **)&m_hLine, (Region<Tile> **)&m_vLine, (Region<Tile> **)&m_subgrid};

    other.m_grid = nullptr;
    other.m_hLine = nullptr;
    other.m_vLine = nullptr;
    other.m_subgrid = nullptr;
}

Tile &Tile::operator=(const Tile &other)
{
    if (this == &other)
        return *this;

    m_grid = other.m_grid;
    m_coordinates = other.m_coordinates;
    m_value = other.m_value;
    m_hLine = other.m_hLine;
    m_vLine = other.m_vLine;
    m_subgrid = other.m_subgrid;
    m_regions = {(Region<Tile> **)&m_hLine, (Region<Tile> **)&m_vLine, (Region<Tile> **)&m_subgrid};

    return *this;
}

Tile &Tile::operator=(Tile &&other)
{
    if (this == &other)
        return *this;

    m_grid = std::move(other.m_grid);
    m_coordinates = std::move(other.m_coordinates);
    m_value = std::move(other.m_value);
    m_hLine = std::move(other.m_hLine);
    m_vLine = std::move(other.m_vLine);
    m_subgrid = std::move(other.m_subgrid);
    m_regions = {(Region<Tile> **)&m_hLine, (Region<Tile> **)&m_vLine, (Region<Tile> **)&m_subgrid};

    other.m_grid = nullptr;
    other.m_hLine = nullptr;
    other.m_vLine = nullptr;
    other.m_subgrid = nullptr;

    return *this;
}
