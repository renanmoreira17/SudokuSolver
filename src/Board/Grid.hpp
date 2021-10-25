#ifndef __GRID_H__
#define __GRID_H__

#include "ElementsContainer.hpp"
#include "GlobalDefinitions.hpp"
#include "Line.hpp"
#include "Subgrid.hpp"

#include <array>
#include <memory>
#include <string>

// class Solver;

class ComponentsConstructor
{
  public:
    virtual std::shared_ptr<Tile> createTile(Grid* grid, TileValueType row, TileValueType col);
    virtual std::shared_ptr<Line> createLine(Grid* grid, LineOrientation row, short index);
    virtual std::shared_ptr<Subgrid> createSubgrid(Grid* grid, short index);

    virtual ~ComponentsConstructor() = default;
};

class Grid
{
  private:
    GridTiles m_gridTiles;
    std::array<std::shared_ptr<Line>, 9> m_verticalLines;
    std::array<std::shared_ptr<Line>, 9> m_horizontalLines;
    std::array<std::shared_ptr<Subgrid>, 9> m_subgrids;

    friend class Solver;
    friend int main();

  public:
    Grid(std::unique_ptr<ComponentsConstructor>&& constructor =
             std::make_unique<ComponentsConstructor>());

    Grid(const std::string& boardStr,
         std::unique_ptr<ComponentsConstructor>&& constructor =
             std::make_unique<ComponentsConstructor>());

    virtual ~Grid() = default;

    std::shared_ptr<Tile> operator()(TileValueType row, TileValueType col);
    const std::shared_ptr<Tile> operator()(TileValueType row, TileValueType col) const;

    GridTiles& getGridTiles() { return m_gridTiles; }
    const GridTiles& getGridTiles() const { return m_gridTiles; }
    std::shared_ptr<Line> getVerticalLine(TileValueType index) { return m_verticalLines[index]; }
    const std::shared_ptr<Line> getVerticalLine(TileValueType index) const
    {
        return m_verticalLines[index];
    }
    std::shared_ptr<Line> getHorizontalLine(TileValueType index)
    {
        return m_horizontalLines[index];
    }
    const std::shared_ptr<Line> getHorizontalLine(TileValueType index) const
    {
        return m_horizontalLines[index];
    }
    std::shared_ptr<Subgrid> getSubgrid(TileValueType index) { return m_subgrids[index]; }
    const std::shared_ptr<Subgrid> getSubgrid(TileValueType index) const
    {
        return m_subgrids[index];
    }
};

#endif // __GRID_H__