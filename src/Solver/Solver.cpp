#include "Solver.hpp"
#include "Board/Tile.hpp"
#include "SolverTile.hpp"
#include "Techniques/HiddenPairs.hpp"
#include "Techniques/HiddenUniqueRectangles.hpp"
#include "Techniques/NakedPairs.hpp"
#include "Techniques/NakedTriples.hpp"
#include "Techniques/PointingPair.hpp"
#include "Techniques/SinglesChains.hpp"
#include "Techniques/SinglesRegion.hpp"
#include "Techniques/SinglesTile.hpp"
#include "Techniques/UniqueRectangles.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <utility>

std::shared_ptr<Tile>
SolverComponentsContructor::createTile(Grid* grid, TileValueType row, TileValueType col) const
{
    return std::static_pointer_cast<Tile>(std::make_shared<SolverTile>(grid, row, col));
}

std::shared_ptr<Line>
SolverComponentsContructor::createLine(Grid* grid, LineOrientation orientation, const short index) const
{
    return std::static_pointer_cast<Line>(std::make_shared<SolverLine>(grid, orientation, index));
}

std::shared_ptr<Subgrid> SolverComponentsContructor::createSubgrid(Grid* grid, const short index) const
{
    return std::static_pointer_cast<Subgrid>(std::make_shared<SolverSubgrid>(grid, index));
}

std::shared_ptr<Tile> SolverComponentsContructor::createTileCopy(Grid* grid,
                                                                 const std::shared_ptr<Tile>& tile) const
{
    return std::make_shared<SolverTile>(*std::dynamic_pointer_cast<SolverTile>(tile), grid);
}
std::shared_ptr<Line> SolverComponentsContructor::createLineCopy(Grid* grid,
                                                                 const std::shared_ptr<Line>& line) const
{
    return std::make_shared<SolverLine>(*std::dynamic_pointer_cast<SolverLine>(line), grid);
}
std::shared_ptr<Subgrid>
SolverComponentsContructor::createSubgridCopy(Grid* grid, const std::shared_ptr<Subgrid>& subgrid) const
{
    return std::make_shared<SolverSubgrid>(*std::dynamic_pointer_cast<SolverSubgrid>(subgrid), grid);
}

Solver::Solver()
    : Grid(std::make_shared<SolverComponentsContructor>())
{
    initialize();
}

Solver::Solver(const std::string& fromBoard)
    : Grid(std::make_shared<SolverComponentsContructor>())
{
    size_t currentIndex = 0;
    size_t currentTileIndex = 0;
    while (currentIndex < fromBoard.size())
    {
        const auto& currentTile = this->operator()(currentTileIndex / 9, currentTileIndex % 9);
        ++currentTileIndex;
        const auto& currentToken = fromBoard[currentIndex];
        switch (currentToken)
        {
        case '{':
        {
            m_initializedWithSuggestions = true;
            const auto endingSuggestionIndex = fromBoard.find('}', currentIndex);
            if (endingSuggestionIndex == std::string::npos)
            {
                throw std::runtime_error(
                    fmt::format("Missing closing bracket for suggestion at tile index {}", currentTileIndex));
            }
            const auto suggestionsString =
                fromBoard.substr(currentIndex + 1, endingSuggestionIndex - currentIndex - 1);
            for (const auto& suggestion : suggestionsString)
            {
                const auto value = static_cast<TileValueType>(suggestion - '0');
                if (value < 1 || value > 9)
                {
                    throw std::runtime_error(
                        fmt::format("Invalid suggestion value {} at tile index {}", value, currentTileIndex));
                }
                currentTile->addSuggestion(value);
            }
            currentIndex = endingSuggestionIndex + 1;
            break;
        }
        default:
        {
            ++currentIndex;
            if (currentToken == '.' || currentToken == '0')
            {
                break;
            }
            const auto value = static_cast<TileValueType>(currentToken - '0');
            if (value < 0 || value > 9)
            {
                throw std::runtime_error(
                    fmt::format("Invalid tile value {} at tile index {}", value, currentTileIndex));
            }
            currentTile->setValue(value);
        }
        }
    }

    initialize();
}

Solver::~Solver() {}

void Solver::computeAllSuggestions(const bool clear)
{
    for (auto& tile : m_gridTiles)
    {
        if (tile->hasValue())
            continue;
        // cast tile to SolverTilePtr and call computeSuggestions
        std::dynamic_pointer_cast<SolverTile>(tile)->computeSuggestions(clear);
    }
}

#define INIT_TECHNIQUE(TechniqueType) m_techniques.emplace_back(std::make_unique<TechniqueType>(*this));

void Solver::initializeTechniques()
{
    // initialize by order of complexity
    INIT_TECHNIQUE(SinglesTile);
    INIT_TECHNIQUE(SinglesRegion);
    INIT_TECHNIQUE(PointingPair);
    INIT_TECHNIQUE(HiddenPairs);
    INIT_TECHNIQUE(NakedPairs);
    INIT_TECHNIQUE(NakedTriples);
    INIT_TECHNIQUE(SinglesChains);
    INIT_TECHNIQUE(UniqueRectangles);
    INIT_TECHNIQUE(HiddenUniqueRectangles);
}

void Solver::solve()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    if (!m_initializedWithSuggestions)
    {
        computeAllSuggestions();
    }
    printGrid();
    // checar enquanto o board não tiver solucionado
    while (!isSolved())
    {
        bool performed = false;
        for (auto&& technique : m_techniques)
        {
            performed = technique->run();
            if (performed)
            {
                break;
            }
            // checar se solucionou
        }
        if (!performed)
        {
            std::cout << "\nNão foi possível resolver o board!" << std::endl;
            return;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "\nBoard resolvido em "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " milisegundos." << std::endl;

    fmt::print("\nTechniques report:\n");
    for (const auto& technique : m_techniques)
    {
        fmt::print("  {:<20}: {}\n", technique->getTechniqueName(), technique->getRanCount());
    }
}

void Solver::initialize()
{
    // initialize techniques
    initializeTechniques();
}

const std::vector<std::shared_ptr<SolverRegion>>& Solver::getAllRegions() const
{
    if (m_allSolverRegions.empty())
    {
        const auto& solverSubgrids = getAllSolverSubgrids();
        m_allSolverRegions.insert(m_allSolverRegions.end(), solverSubgrids.begin(), solverSubgrids.end());
        const auto& solverHorizontalLines = getAllSolverHorizontalLines();
        m_allSolverRegions.insert(
            m_allSolverRegions.end(), solverHorizontalLines.begin(), solverHorizontalLines.end());
        const auto& solverVerticalLines = getAllSolverVerticalLines();
        m_allSolverRegions.insert(
            m_allSolverRegions.end(), solverVerticalLines.begin(), solverVerticalLines.end());
    }
    return m_allSolverRegions;
}

const std::vector<std::shared_ptr<SolverSubgrid>>& Solver::getAllSolverSubgrids() const
{
    if (m_allSolverSubgrids.empty())
    {
        const auto& subgrids = getSubgrids();
        std::transform(subgrids.begin(),
                       subgrids.end(),
                       std::back_inserter(m_allSolverSubgrids),
                       [](const auto& subgrid) { return std::dynamic_pointer_cast<SolverSubgrid>(subgrid); });
    }
    return m_allSolverSubgrids;
}

const std::vector<std::shared_ptr<SolverLine>>& Solver::getAllSolverHorizontalLines() const
{
    if (m_allSolverHorizontalLines.empty())
    {
        const auto& horizontalLines = getHorizontalLines();
        std::transform(horizontalLines.begin(),
                       horizontalLines.end(),
                       std::back_inserter(m_allSolverHorizontalLines),
                       [](const auto& line) { return std::dynamic_pointer_cast<SolverLine>(line); });
    }
    return m_allSolverHorizontalLines;
}

const std::vector<std::shared_ptr<SolverLine>>& Solver::getAllSolverVerticalLines() const
{
    if (m_allSolverVerticalLines.empty())
    {
        const auto& verticalLines = getVerticalLines();
        std::transform(verticalLines.begin(),
                       verticalLines.end(),
                       std::back_inserter(m_allSolverVerticalLines),
                       [](const auto& line) { return std::dynamic_pointer_cast<SolverLine>(line); });
    }
    return m_allSolverVerticalLines;
}

std::vector<std::string> Solver::requestTileDisplayStringForCoordinate(const TileValueType row,
                                                                       const TileValueType col) const
{
    const auto solverTile = std::dynamic_pointer_cast<SolverTile>(m_gridTiles(row, col));
    if (solverTile->hasValue())
    {
        return Grid::requestTileDisplayStringForCoordinate(row, col);
    }

    std::vector<std::string> result;
    for (TileValueType row = 0; row < 3; row++)
    {
        std::stringstream line;
        for (TileValueType col = 0; col < 3; col++)
        {
            if (solverTile->hasSuggestion(row * 3 + col + 1))
            {
                line << (row * 3 + col + 1);
            }
            else
            {
                line << " ";
            }

            if (col < 2)
            {
                line << " ";
            }
        }
        result.emplace_back(line.str());
    }

    return result;
}

SolverTilePtr Solver::operator()(TileValueType row, TileValueType col) const
{
    return std::dynamic_pointer_cast<SolverTile>(Grid::operator()(row, col));
}
SolverTilePtr Solver::operator()(const Coordinates& coordinates) const
{
    return std::dynamic_pointer_cast<SolverTile>(Grid::operator()(coordinates));
}
