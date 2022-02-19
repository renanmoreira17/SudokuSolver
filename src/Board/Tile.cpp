#include "Tile.hpp"
#include "Line.hpp"
#include "Region.hpp"
#include "Subgrid.hpp"
#include <utility>

Tile::Tile(Grid* grid, Coordinates coordinates, const TileValueType value)
    : m_grid(grid)
    , m_coordinates(coordinates)
    , m_value(value)
{}

Tile::Tile(const Tile& other)
{
    m_grid = nullptr;
    m_coordinates = other.m_coordinates;
    m_value = other.m_value;
    m_horizontalLine = nullptr;
    m_verticalLine = nullptr;
    m_subgrid = nullptr;
}

Tile::Tile(const Tile& other, Grid* grid)
{
    m_grid = grid;
    m_coordinates = other.m_coordinates;
    m_value = other.m_value;
    m_horizontalLine = nullptr;
    m_verticalLine = nullptr;
    m_subgrid = nullptr;
}

const Coordinates& Tile::getCoordinates() const
{
    return m_coordinates;
}

void Tile::setValue(const TileValueType value)
{
    m_value = value;
}

TileValueType Tile::getValue() const
{
    return m_value;
}

bool Tile::hasValue() const
{
    return m_value != 0;
}

void Tile::setLine(LineOrientation orientation, Line* line)
{
    if (orientation == LineOrientation::HORIZONTAL)
    {
        setHorizontalLine(line);
    }
    else
    {
        setVerticalLine(line);
    }
}

void Tile::setHorizontalLine(Line* const& hLine) const
{
    m_horizontalLine = hLine;
}

Line* Tile::getHorizontalLine() const
{
    return m_horizontalLine;
}

void Tile::setVerticalLine(Line* const& vLine) const
{
    m_verticalLine = vLine;
}

Line* Tile::getVerticalLine() const
{
    return m_verticalLine;
}

void Tile::setSubgrid(Subgrid* const& subgrid) const
{
    m_subgrid = subgrid;
}

Subgrid* Tile::getSubgrid() const
{
    return m_subgrid;
}

const std::tuple<Line*, Line*, Subgrid*> Tile::getRegionsTuple()
{
    return {getHorizontalLine(), getVerticalLine(), getSubgrid()};
}

const std::tuple<const Line*, const Line*, const Subgrid*> Tile::getRegionsTuple() const
{
    return {getHorizontalLine(), getVerticalLine(), getSubgrid()};
}

std::array<Region*, 3> Tile::getRegions() const
{
    return {getHorizontalLine(), getVerticalLine(), getSubgrid()};
}

bool Tile::operator==(const TileValueType value) const
{
    return m_value == value;
}

Tile& Tile::operator=(const TileValueType value)
{
    setValue(value);
    return *this;
}

void Tile::setGrid(Grid* grid)
{
    m_grid = grid;
}

Grid* Tile::getGrid() const
{
    return m_grid;
}
