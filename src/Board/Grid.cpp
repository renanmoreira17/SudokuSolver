#include "Grid.hpp"
#include "Tile.hpp"
#include "Line.hpp"

Grid::Grid()
{
    for (TileValueType row = 0; row < 9; ++row)
        for (TileValueType col = 0; col < 9; ++col)
            m_gridTiles(row, col) = Tile(this, {row, col});

    for (TileValueType i = 0; i < 9; i++) {
        m_verticalLines[i] = Line(this, vertical, i);
        m_horizontalLines[i] = Line(this, horizontal, i);
        m_subgrids[i] = Subgrid(this, i);
    }
}

Grid::Grid(const std::string& boardStr)
: Grid()
{
    for (std::string::size_type i = 0; i < boardStr.size(); i++)
    {
        if (boardStr[i] == '.') continue;
        const TileValueType col = i % 9;
        const TileValueType row = i / 9;
        const TileValueType value = boardStr[i] - '0';

        m_gridTiles(row, col) = value;
    }
}

Tile& Grid::operator()(TileValueType row, TileValueType col)
{
    return m_gridTiles(row, col);
}

const Tile& Grid::operator()(TileValueType row, TileValueType col) const
{
    return m_gridTiles(row, col);
}
