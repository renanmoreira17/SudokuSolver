#include "SolverRegions.hpp"
#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"
#include "Solver.hpp"

#include <algorithm>
#include <exception>
#include <optional>
#include <utility>

SolverRegion::SolverRegion(Grid* grid, const unsigned index, const RegionType type)
    : Region(index, type)
    , m_grid(grid)
{
    for (TileValueType i = 1; i <= 9; i++) { m_suggestionsQuan[i] = 0; }
}

SolverRegion::SolverRegion(SolverRegion&& other)
    : Region(other)
{
    m_grid = other.m_grid;
    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_grid = nullptr;
    other.m_elementList = nullptr;
}

Solver* SolverRegion::getSolver() const
{
    // try casting m_grid to Solver*, and return it if it is valid, otherwise throw an exception
    Solver* solver = dynamic_cast<Solver*>(m_grid);
    if (solver == nullptr)
    {
        throw std::runtime_error("SolverRegion::getSolver() - m_grid is not a Solver*");
    }
    return solver;
}

SolverRegion& SolverRegion::operator=(SolverRegion&& other)
{
    if (&other == this)
        return *this;

    Region::operator=(std::move(other));

    m_grid = other.m_grid;
    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_grid = nullptr;
    other.m_elementList = nullptr;

    return *this;
}

const std::vector<std::shared_ptr<SolverTile>>& SolverRegion::getSolverTiles() const
{
    if (m_solverTiles == nullptr)
    {
        m_solverTiles = std::make_unique<std::vector<std::shared_ptr<SolverTile>>>();
        for (const auto& tile : getTiles())
        {
            m_solverTiles->emplace_back(std::dynamic_pointer_cast<SolverTile>(tile));
        }
    }
    return *m_solverTiles;
}

void SolverRegion::suggestionAdded(const unsigned value)
{
    auto& number = m_suggestionsQuan[value];
    if (number == 9)
        throw std::out_of_range("Trying to increase suggestions number, but it's already at 9!");
    ++number;
}

void SolverRegion::suggestionRemoved(const unsigned value)
{
    auto& number = m_suggestionsQuan[value];
    if (number == 0)
        throw std::out_of_range("Trying to decrease suggestions number, but it's already at 0!");
    --number;
}

TileValueType SolverRegion::getSuggestionsQuanFor(TileValueType value) const
{
    if (value > 9)
        throw std::out_of_range("Trying to get suggestions number for a value greater than 9!");
    return m_suggestionsQuan.at(value);
}

bool SolverRegion::removeSuggestionsFromTiles(
    const std::vector<TileValueType>& values,
    const std::optional<std::vector<std::shared_ptr<SolverTile>>>& exceptFromTiles)
{
    bool performed = false;
    for (const std::shared_ptr<SolverTile>& solverTile : getSolverTiles())
    {
        if (solverTile->getSuggestions().empty() ||
            (exceptFromTiles.has_value() &&
             std::any_of(
                 exceptFromTiles->begin(),
                 exceptFromTiles->end(),
                 [&solverTile](const std::shared_ptr<SolverTile>& t) { return t == solverTile; })))
        {
            continue;
        }

        for (const auto& currentSuggestion : values)
        {
            const bool erased = solverTile->removeSuggestion(currentSuggestion);
            performed |= erased;
        }
    }

    return performed;
}

std::vector<std::shared_ptr<SolverTile>>
SolverRegion::getTilesWithSuggestion(TileValueType value) const
{
    std::vector<std::shared_ptr<SolverTile>> tiles;
    for (const std::shared_ptr<SolverTile>& solverTile : getSolverTiles())
    {
        if (solverTile->getSuggestions().empty())
            continue;

        if (solverTile->hasSuggestion(value))
            tiles.emplace_back(solverTile);
    }
    return tiles;
}

// SPECIFIC REGIONS

SolverLine::SolverLine(Grid* grid, LineOrientation orientation, const short index)
    : Region(index, RegionType::LINE)
    , SolverRegion(grid, index, RegionType::LINE)
    , Line(grid, orientation, index)
{}

SolverSubgrid::SolverSubgrid(Grid* grid, const short index)
    : Region(index, RegionType::SUBGRID)
    , SolverRegion(grid, index, RegionType::SUBGRID)
    , Subgrid(grid, index)
{}
