#include "Solver.hpp"
#include "Board/Tile.hpp"
#include "SolverTile.hpp"
#include "Techniques/NakedPairs.hpp"
#include "Techniques/Singles.hpp"

#include <algorithm>
#include <array>
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

void Solver::solve()
{
    computeAllSuggestions();
    // checar enquanto o board não tiver solucionado
    while (true)
    {
        for (auto&& technique : m_techniques)
        {
            if (technique->run())
                break;
            // checar se solucionou
        }
    }
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
    m_techniques.emplace_back(std::make_unique<NakedPairs>(this));
    m_techniques.emplace_back(std::make_unique<Singles>(this));
}
