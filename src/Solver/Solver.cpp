#include "Solver.hpp"
#include "Board/Tile.hpp"
#include "SolverTile.hpp"
#include "Techniques/NakedPairs.hpp"
#include "Techniques/SinglesRegion.hpp"
#include "Techniques/SinglesTile.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <utility>

std::shared_ptr<Tile>
SolverComponentsContructor::createTile(Grid* grid, TileValueType row, TileValueType col)
{
    return std::static_pointer_cast<Tile>(std::make_shared<SolverTile>(grid, row, col));
}

std::shared_ptr<Line>
SolverComponentsContructor::createLine(Grid* grid, LineOrientation orientation, const short index)
{
    return std::static_pointer_cast<Line>(std::make_shared<SolverLine>(grid, orientation, index));
}

std::shared_ptr<Subgrid> SolverComponentsContructor::createSubgrid(Grid* grid, const short index)
{
    return std::static_pointer_cast<Subgrid>(std::make_shared<SolverSubgrid>(grid, index));
}

Solver::Solver()
    : Grid(std::make_unique<SolverComponentsContructor>())
{
    initialize();
}

Solver::Solver(const std::string& fromBoard)
    : Grid(fromBoard, std::make_unique<SolverComponentsContructor>())
{
    initialize();
}

Solver::~Solver() {}

void Solver::computeAllSuggestions(const bool clear)
{
    for (auto& tile : m_gridTiles)
    {
        if (tile->hasValue())
            continue;
        // cast tile to std::shared_ptr<SolverTile> and call computeSuggestions
        std::dynamic_pointer_cast<SolverTile>(tile)->computeSuggestions(clear);
    }
}

void Solver::initializeTechniques()
{
    // initialize by order of complexity
    m_techniques.push_back(std::make_unique<SinglesTile>(*this));
    m_techniques.push_back(std::make_unique<SinglesRegion>(*this));
    m_techniques.push_back(std::make_unique<NakedPairs>(*this));
}

void Solver::solve()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    computeAllSuggestions();
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
}

void Solver::initialize()
{
    for (TileValueType i = 0; i < 9; i++)
    {
        m_allRegions[i * 3 + 0] = std::dynamic_pointer_cast<SolverRegion>(m_horizontalLines[i]);
        m_allRegions[i * 3 + 1] = std::dynamic_pointer_cast<SolverRegion>(m_verticalLines[i]);
        m_allRegions[i * 3 + 2] = std::dynamic_pointer_cast<SolverRegion>(m_subgrids[i]);
    }

    // initialize techniques
    initializeTechniques();
}

std::vector<std::string>
Solver::requestTileDisplayStringForCoordinate(const TileValueType row,
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
