#ifndef __GRID_H__
#define __GRID_H__

#include "ElementsContainer.hpp"
#include "Line.hpp"
#include "Subgrid.hpp"
#include <string>
#include <iterator>
#include "GlobalDefinitions.hpp"


// class Solver;

class Grid
{
private:
    GridTiles m_gridTiles;
    Line m_verticalLines[9];
    Line m_horizontalLines[9];
    Subgrid m_subgrids[9];

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