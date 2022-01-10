#ifndef __GRID_H__
#define __GRID_H__

#include "ElementsContainer.hpp"
#include "GlobalDefinitions.hpp"

#include <memory>
#include <string>
#include <vector>

class Line;
class Subgrid;
class GridPrinter;

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
    std::vector<std::shared_ptr<Line>> m_verticalLines;
    std::vector<std::shared_ptr<Line>> m_horizontalLines;
    std::vector<std::shared_ptr<Subgrid>> m_subgrids;

    friend class Solver;
    friend class GridPrinter;
    friend int main();

  public:
    void print() const;

  protected:
    virtual std::vector<std::string>
    requestTileDisplayStringForCoordinate(const TileValueType row, const TileValueType col) const;

    std::unique_ptr<GridPrinter> m_gridPrinter;

    void printGrid() const;

  public:
    Grid(std::unique_ptr<ComponentsConstructor>&& constructor =
             std::make_unique<ComponentsConstructor>());

    Grid(const std::string& boardStr,
         std::unique_ptr<ComponentsConstructor>&& constructor =
             std::make_unique<ComponentsConstructor>());

    Grid(const Grid& other);
    Grid(Grid&& other);
    Grid& operator=(const Grid& other);
    Grid& operator=(Grid&& other);

    virtual ~Grid();

    std::shared_ptr<Tile> operator()(TileValueType row, TileValueType col);
    const std::shared_ptr<Tile> operator()(TileValueType row, TileValueType col) const;

    GridTiles& getGridTiles() { return m_gridTiles; }
    const GridTiles& getGridTiles() const { return m_gridTiles; }

    const std::vector<std::shared_ptr<Line>>& getVerticalLines() const { return m_verticalLines; }
    std::shared_ptr<Line> getVerticalLine(TileValueType index) { return m_verticalLines[index]; }
    const std::shared_ptr<Line> getVerticalLine(TileValueType index) const
    {
        return m_verticalLines[index];
    }

    const std::vector<std::shared_ptr<Line>>& getHorizontalLines() const
    {
        return m_horizontalLines;
    }
    std::shared_ptr<Line> getHorizontalLine(TileValueType index)
    {
        return m_horizontalLines[index];
    }
    const std::shared_ptr<Line> getHorizontalLine(TileValueType index) const
    {
        return m_horizontalLines[index];
    }

    const std::vector<std::shared_ptr<Subgrid>>& getSubgrids() const { return m_subgrids; }
    std::shared_ptr<Subgrid> getSubgrid(TileValueType index) { return m_subgrids[index]; }
    const std::shared_ptr<Subgrid> getSubgrid(TileValueType index) const
    {
        return m_subgrids[index];
    }
};

#endif // __GRID_H__