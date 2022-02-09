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
        m_verticalLines.emplace_back(constructor->createLine(this, LineOrientation::VERTICAL, i));
        m_horizontalLines.emplace_back(
            constructor->createLine(this, LineOrientation::HORIZONTAL, i));
        m_subgrids.emplace_back(constructor->createSubgrid(this, i));
    }

    m_gridPrinter = std::make_unique<GridPrinter>(*this);
}

Grid::Grid(const std::string& boardStr, std::unique_ptr<ComponentsConstructor>&& constructor)
    : Grid(std::move(constructor))
{
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
    m_gridTiles = other.m_gridTiles;
    m_verticalLines = other.m_verticalLines;
    m_horizontalLines = other.m_horizontalLines;
    m_subgrids = other.m_subgrids;
    m_gridPrinter = std::make_unique<GridPrinter>(*this);
}

Grid::Grid(Grid&& other)
{
    m_gridTiles = std::move(other.m_gridTiles);
    m_verticalLines = std::move(other.m_verticalLines);
    m_horizontalLines = std::move(other.m_horizontalLines);
    m_subgrids = std::move(other.m_subgrids);
    m_gridPrinter = std::make_unique<GridPrinter>(*this);
}

Grid& Grid::operator=(const Grid& other)
{
    m_gridTiles = other.m_gridTiles;
    m_verticalLines = other.m_verticalLines;
    m_horizontalLines = other.m_horizontalLines;
    m_subgrids = other.m_subgrids;
    m_gridPrinter = std::make_unique<GridPrinter>(*this);
    return *this;
}

Grid& Grid::operator=(Grid&& other)
{
    m_gridTiles = std::move(other.m_gridTiles);
    m_verticalLines = std::move(other.m_verticalLines);
    m_horizontalLines = std::move(other.m_horizontalLines);
    m_subgrids = std::move(other.m_subgrids);
    m_gridPrinter = std::make_unique<GridPrinter>(*this);
    return *this;
}

Grid::~Grid(){};

std::shared_ptr<Tile> Grid::operator()(TileValueType row, TileValueType col)
{
    return m_gridTiles(row, col);
}

const std::shared_ptr<Tile> Grid::operator()(TileValueType row, TileValueType col) const
{
    return m_gridTiles(row, col);
}

void Grid::print() const
{
    for (TileValueType row = 0; row < 9; ++row)
    {
        for (TileValueType col = 0; col < 9; ++col)
        {

            std::cout << m_gridTiles(row, col)->getValue() << " ";
        }
    }
}

std::vector<std::string> Grid::requestTileDisplayStringForCoordinate(const TileValueType row,
                                                                     const TileValueType col) const
{
    const auto& tile = m_gridTiles(row, col);
    std::vector<std::string> result;
    result.emplace_back("\\   /");
    result.emplace_back(
        fmt::format("  {}  ", tile->hasValue() ? std::to_string(tile->getValue()) : " "));
    result.emplace_back("/   \\");

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
