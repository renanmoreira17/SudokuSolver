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
