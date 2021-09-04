#ifndef __GRID_H__
#define __GRID_H__

#include "ElementsContainer.hpp"
#include "Line.hpp"
#include "Subgrid.hpp"
#include "GlobalDefinitions.hpp"

#include <string>
#include <array>

// class Solver;

class Grid
{
private:
    GridTiles m_gridTiles;
    std::array<Line, 9> m_verticalLines;
    std::array<Line, 9> m_horizontalLines;
    std::array<Subgrid, 9> m_subgrids;

    friend class Solver;
    friend int main();
public:
    Grid();
    Grid(const std::string& boardStr);
    ~Grid() {}

    Tile& operator()(TileValueType row, TileValueType col);
    const Tile& operator()(TileValueType row, TileValueType col) const;

    GridTiles& getGridTiles() { return m_gridTiles; }
    const GridTiles& getGridTiles() const { return m_gridTiles; }
    Line& getVerticalLine(TileValueType index) { return m_verticalLines[index]; }
    const Line& getVerticalLine(TileValueType index) const { return m_verticalLines[index]; }
    Line& getHorizontalLine(TileValueType index) { return m_horizontalLines[index]; }
    const Line& getHorizontalLine(TileValueType index) const { return m_horizontalLines[index]; }
    Subgrid& getSubgrid(TileValueType index) { return m_subgrids[index]; }
    const Subgrid& getSubgrid(TileValueType index) const { return m_subgrids[index]; }
};

#endif // __GRID_H__