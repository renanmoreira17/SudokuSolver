#include "Grid.hpp"
#include "GridPrinter.hpp"
#include "Line.hpp"
#include "Subgrid.hpp"
#include "Tile.hpp"

#include <fmt/format.h>
#include <iostream>
#include <string>
#include <vector>

std::shared_ptr<Tile>
DefaultComponentsConstructor::createTile(Grid* grid, TileValueType row, TileValueType col) const
{
    return std::make_shared<Tile>(grid, Coordinates{row, col});
}

std::shared_ptr<Line>
DefaultComponentsConstructor::createLine(Grid* grid, LineOrientation orientation, const short index) const
{
    return std::make_shared<Line>(grid, orientation, index);
}

std::shared_ptr<Subgrid> DefaultComponentsConstructor::createSubgrid(Grid* grid, const short index) const
{
    return std::make_shared<Subgrid>(grid, index);
}

std::shared_ptr<Tile> DefaultComponentsConstructor::createTileCopy(Grid* grid,
                                                                   const std::shared_ptr<Tile>& tile) const
{
    return std::make_shared<Tile>(*tile, grid);
}

std::shared_ptr<Line> DefaultComponentsConstructor::createLineCopy(Grid* grid,
                                                                   const std::shared_ptr<Line>& line) const
{
    return std::make_shared<Line>(*line, grid);
}
std::shared_ptr<Subgrid>
DefaultComponentsConstructor::createSubgridCopy(Grid* grid, const std::shared_ptr<Subgrid>& subgrid) const
{
    return std::make_shared<Subgrid>(*subgrid, grid);
}

Grid::Grid(std::shared_ptr<ComponentsConstructor>&& constructor)
    : m_constructor(std::move(constructor))
{
    for (TileValueType row = 0; row < 9; ++row)
        for (TileValueType col = 0; col < 9; ++col)
            m_gridTiles(row, col) = m_constructor->createTile(this, row, col);

    for (TileValueType i = 0; i < 9; i++)
    {
        m_verticalLines.emplace_back(m_constructor->createLine(this, LineOrientation::VERTICAL, i));
        m_horizontalLines.emplace_back(m_constructor->createLine(this, LineOrientation::HORIZONTAL, i));
        m_subgrids.emplace_back(m_constructor->createSubgrid(this, i));
    }

    m_gridPrinter = std::make_unique<GridPrinter>(*this);
}

Grid::Grid(const std::string& boardStr, std::shared_ptr<ComponentsConstructor>&& constructor)
    : Grid(std::move(constructor))
{
    fmt::print("Grid::Grid(const std::string& boardStr)\n");
    for (std::string::size_type i = 0; i < boardStr.size(); i++)
    {
        if (boardStr[i] == '.' || boardStr[i] == '0')
            continue;
        const TileValueType col = i % 9;
        const TileValueType row = i / 9;
        const TileValueType value = boardStr[i] - '0';

        m_gridTiles(row, col)->setValue(value);
    }
}

Grid::Grid(const Grid& other)
{
    m_constructor = other.m_constructor;

    for (TileValueType row = 0; row < 9; ++row)
        for (TileValueType col = 0; col < 9; ++col)
            m_gridTiles(row, col) = m_constructor->createTileCopy(this, other.m_gridTiles(row, col));

    for (TileValueType i = 0; i < 9; i++)
    {
        m_verticalLines.emplace_back(m_constructor->createLineCopy(this, other.m_verticalLines[i]));
        m_horizontalLines.emplace_back(m_constructor->createLineCopy(this, other.m_horizontalLines[i]));
        m_subgrids.emplace_back(m_constructor->createSubgridCopy(this, other.m_subgrids[i]));
    }

    m_gridPrinter = std::make_unique<GridPrinter>(*this);
}

Grid::Grid(Grid&& other)
{
    m_gridTiles = std::move(other.m_gridTiles);
    for (const auto& tile : m_gridTiles) tile->setGrid(this);

    m_verticalLines = std::move(other.m_verticalLines);
    for (const auto& line : m_verticalLines) line->setGrid(this);

    m_horizontalLines = std::move(other.m_horizontalLines);
    for (const auto& line : m_horizontalLines) line->setGrid(this);

    m_subgrids = std::move(other.m_subgrids);
    for (const auto& subgrid : m_subgrids) subgrid->setGrid(this);

    m_gridPrinter = std::make_unique<GridPrinter>(*this);

    m_constructor = std::move(other.m_constructor);
}

Grid& Grid::operator=(const Grid& other)
{
    if (this == &other)
        return *this;
    m_constructor = other.m_constructor;

    for (TileValueType row = 0; row < 9; ++row)
        for (TileValueType col = 0; col < 9; ++col)
            m_gridTiles(row, col) = m_constructor->createTileCopy(this, other.m_gridTiles(row, col));

    for (TileValueType i = 0; i < 9; i++)
    {
        m_verticalLines[i] = m_constructor->createLineCopy(this, other.m_verticalLines[i]);
        m_horizontalLines[i] = m_constructor->createLineCopy(this, other.m_horizontalLines[i]);
        m_subgrids[i] = m_constructor->createSubgridCopy(this, other.m_subgrids[i]);
    }

    return *this;
}

Grid& Grid::operator=(Grid&& other)
{
    if (this == &other)
        return *this;

    m_gridTiles = std::move(other.m_gridTiles);
    for (const auto& tile : m_gridTiles) tile->setGrid(this);

    m_verticalLines = std::move(other.m_verticalLines);
    for (const auto& line : m_verticalLines) line->setGrid(this);

    m_horizontalLines = std::move(other.m_horizontalLines);
    for (const auto& line : m_horizontalLines) line->setGrid(this);

    m_subgrids = std::move(other.m_subgrids);
    for (const auto& subgrid : m_subgrids) subgrid->setGrid(this);

    m_constructor = std::move(other.m_constructor);

    return *this;
}

Grid::~Grid(){};

const std::shared_ptr<Tile>& Grid::operator()(TileValueType row, TileValueType col) const
{
    return m_gridTiles(row, col);
}

const std::shared_ptr<Tile>& Grid::operator()(const Coordinates& coordinates) const
{
    return m_gridTiles(coordinates.row, coordinates.col);
}

void Grid::print() const
{
    for (TileValueType row = 0; row < 9; ++row)
    {
        for (TileValueType col = 0; col < 9; ++col) { std::cout << m_gridTiles(row, col)->getValue() << " "; }
    }
}

std::string Grid::getBoardString() const
{
    return m_gridPrinter->createBoardString();
}

std::vector<std::string> Grid::requestTileDisplayStringForCoordinate(const TileValueType row,
                                                                     const TileValueType col) const
{
    const auto& tile = m_gridTiles(row, col);
    std::vector<std::string> result;
    result.emplace_back(fmt::format(R"(\   /)"));
    result.emplace_back(fmt::format(R"(  {}  )", tile->hasValue() ? std::to_string(tile->getValue()) : " "));
    result.emplace_back(fmt::format(R"(/   \)"));

    return result;
}

void Grid::printGrid() const
{
    std::cout << m_gridPrinter->createBoardString() << std::endl;
}

#ifdef DEBUG
#include <sstream>
void Grid::printGridDebug() const
{
    const auto boardString = m_gridPrinter->createBoardString();
    auto ss = std::stringstream{boardString};

    for (std::string line; std::getline(ss, line, '\n');)
    {
        std::replace(line.begin(), line.end(), '\\', '|');
        std::cout << line << "\n";
    }
}
#endif

bool Grid::isSolved() const
{
    for (const auto& tile : m_gridTiles)
    {
        if (!tile->hasValue())
            return false;
    }
    return true;
}

GridTiles& Grid::getGridTiles()
{
    return m_gridTiles;
}
const GridTiles& Grid::getGridTiles() const
{
    return m_gridTiles;
}

const std::vector<std::shared_ptr<Line>>& Grid::getVerticalLines() const
{
    return m_verticalLines;
}
std::shared_ptr<Line> Grid::getVerticalLine(TileValueType index)
{
    return m_verticalLines[index];
}
const std::shared_ptr<Line> Grid::getVerticalLine(TileValueType index) const
{
    return m_verticalLines[index];
}

const std::vector<std::shared_ptr<Line>>& Grid::getHorizontalLines() const
{
    return m_horizontalLines;
}
std::shared_ptr<Line> Grid::getHorizontalLine(TileValueType index)
{
    return m_horizontalLines[index];
}
const std::shared_ptr<Line> Grid::getHorizontalLine(TileValueType index) const
{
    return m_horizontalLines[index];
}

const std::vector<std::shared_ptr<Subgrid>>& Grid::getSubgrids() const
{
    return m_subgrids;
}
std::shared_ptr<Subgrid> Grid::getSubgrid(TileValueType index)
{
    return m_subgrids[index];
}
const std::shared_ptr<Subgrid> Grid::getSubgrid(TileValueType index) const
{
    return m_subgrids[index];
}

const std::shared_ptr<ComponentsConstructor>& Grid::getComponentsConstructor() const
{
    return m_constructor;
}
