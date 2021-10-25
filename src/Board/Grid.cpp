#include "Grid.hpp"
#include "Line.hpp"
#include "Tile.hpp"

std::shared_ptr<Tile>
ComponentsConstructor::createTile(Grid* grid, TileValueType row, TileValueType col)
{
    return std::make_shared<Tile>(grid, Coordinates{row, col});
}

std::shared_ptr<Line>
ComponentsConstructor::createLine(Grid* grid, LineOrientation orientation, const short index)
{
    return std::make_shared<Line>(grid, orientation, index);
}

std::shared_ptr<Subgrid> ComponentsConstructor::createSubgrid(Grid* grid, const short index)
{
    return std::make_shared<Subgrid>(grid, index);
}

Grid::Grid(std::unique_ptr<ComponentsConstructor>&& constructor)
{
    for (TileValueType row = 0; row < 9; ++row)
        for (TileValueType col = 0; col < 9; ++col)
            m_gridTiles(row, col) = constructor->createTile(this, row, col);

    for (TileValueType i = 0; i < 9; i++)
    {
        m_verticalLines[i] = constructor->createLine(this, LineOrientation::VERTICAL, i);
        m_horizontalLines[i] = constructor->createLine(this, LineOrientation::HORIZONTAL, i);
        m_subgrids[i] = constructor->createSubgrid(this, i);
    }
}

Grid::Grid(const std::string& boardStr, std::unique_ptr<ComponentsConstructor>&& constructor)
    : Grid(std::move(constructor))
{
    for (std::string::size_type i = 0; i < boardStr.size(); i++)
    {
        if (boardStr[i] == '.')
            continue;
        const TileValueType col = i % 9;
        const TileValueType row = i / 9;
        const TileValueType value = boardStr[i] - '0';

        m_gridTiles(row, col)->setValue(value);
    }
}

std::shared_ptr<Tile> Grid::operator()(TileValueType row, TileValueType col)
{
    return m_gridTiles(row, col);
}

const std::shared_ptr<Tile> Grid::operator()(TileValueType row, TileValueType col) const
{
    return m_gridTiles(row, col);
}
