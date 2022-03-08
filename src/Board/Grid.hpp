#ifndef __GRID_H__
#define __GRID_H__

#include "ElementsContainer.hpp"
#include "Util/GlobalDefinitions.hpp"

#include <memory>
#include <string>
#include <vector>

class Line;
class Subgrid;
class GridPrinter;

class ComponentsConstructor
{
  public:
    virtual std::shared_ptr<Tile> createTile(Grid* grid, TileValueType row, TileValueType col) const = 0;
    virtual std::shared_ptr<Line> createLine(Grid* grid, LineOrientation row, short index) const = 0;
    virtual std::shared_ptr<Subgrid> createSubgrid(Grid* grid, short index) const = 0;

    virtual std::shared_ptr<Tile> createTileCopy(Grid* grid, const std::shared_ptr<Tile>& tile) const = 0;
    virtual std::shared_ptr<Line> createLineCopy(Grid* grid, const std::shared_ptr<Line>& line) const = 0;
    virtual std::shared_ptr<Subgrid> createSubgridCopy(Grid* grid,
                                                       const std::shared_ptr<Subgrid>& subgrid) const = 0;

    virtual ~ComponentsConstructor() = default;
};

class DefaultComponentsConstructor : public ComponentsConstructor
{
  public:
    std::shared_ptr<Tile> createTile(Grid* grid, TileValueType row, TileValueType col) const override;
    std::shared_ptr<Line> createLine(Grid* grid, LineOrientation row, short index) const override;
    std::shared_ptr<Subgrid> createSubgrid(Grid* grid, short index) const override;

    std::shared_ptr<Tile> createTileCopy(Grid* grid, const std::shared_ptr<Tile>& tile) const override;
    std::shared_ptr<Line> createLineCopy(Grid* grid, const std::shared_ptr<Line>& line) const override;
    std::shared_ptr<Subgrid> createSubgridCopy(Grid* grid,
                                               const std::shared_ptr<Subgrid>& subgrid) const override;
};

class Grid
{
  private:
    GridTiles m_gridTiles;
    std::vector<std::shared_ptr<Line>> m_verticalLines;
    std::vector<std::shared_ptr<Line>> m_horizontalLines;
    std::vector<std::shared_ptr<Subgrid>> m_subgrids;

    std::shared_ptr<ComponentsConstructor> m_constructor;

    friend class Solver;
    friend class GridPrinter;
    friend int main();

  public:
    void print() const;
    std::string getBoardString() const;

  protected:
    virtual std::vector<std::string> requestTileDisplayStringForCoordinate(const TileValueType row,
                                                                           const TileValueType col) const;

    std::unique_ptr<GridPrinter> m_gridPrinter;

    void printGrid() const;

#ifdef DEBUG
  public:
    void printGridDebug() const;

#endif

  public:
    Grid(std::shared_ptr<ComponentsConstructor>&& constructor =
             std::make_shared<DefaultComponentsConstructor>());

    Grid(const std::string& boardStr,
         std::shared_ptr<ComponentsConstructor>&& constructor =
             std::make_shared<DefaultComponentsConstructor>());

    Grid(const Grid& other);
    Grid(Grid&& other);
    Grid& operator=(const Grid& other);
    Grid& operator=(Grid&& other);

    virtual ~Grid();

    const std::shared_ptr<Tile>& operator()(TileValueType row, TileValueType col) const;
    const std::shared_ptr<Tile>& operator()(const Coordinates& coordinates) const;

    GridTiles& getGridTiles();
    const GridTiles& getGridTiles() const;

    const std::vector<std::shared_ptr<Line>>& getVerticalLines() const;
    std::shared_ptr<Line> getVerticalLine(TileValueType index);
    const std::shared_ptr<Line> getVerticalLine(TileValueType index) const;

    const std::vector<std::shared_ptr<Line>>& getHorizontalLines() const;
    std::shared_ptr<Line> getHorizontalLine(TileValueType index);
    const std::shared_ptr<Line> getHorizontalLine(TileValueType index) const;

    const std::vector<std::shared_ptr<Subgrid>>& getSubgrids() const;
    std::shared_ptr<Subgrid> getSubgrid(TileValueType index);
    const std::shared_ptr<Subgrid> getSubgrid(TileValueType index) const;

    const std::shared_ptr<ComponentsConstructor>& getComponentsConstructor() const;

    bool isSolved() const;
};

#endif // __GRID_H__