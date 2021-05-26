#include "SolverRegions.hpp"
#include <optional>
#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"
#include "Solver.hpp"
#include <exception>
#include <algorithm>
#include <utility>


template<>
bool Region<std::pair<Tile*, Suggestions>>::hasValue(const TileValueType value) const {
    for (auto &&element : *m_elementList)
    {
        if (element.second.find(value) != element.second.cend())
        {
            return true;
        }
    }
    return false;
}

template<>
const Tile& Region<std::pair<Tile*, Suggestions>>::getTileFromT(const std::pair<Tile*, Suggestions>& element) const
{
    return *element.first;
}


SolverRegion::SolverRegion(Solver* const& solver, const unsigned index, Region<Tile>* const& originalRegion)
    : Region(index), m_solver(solver), m_originalRegion(originalRegion)
{
    for (TileValueType i = 1; i <= 9; i++)
    {
        m_suggestionsQuan[i] = 0;
    }
}

SolverRegion::SolverRegion(SolverRegion&& other)
: Region(other)
{
    m_solver = other.m_solver;
    m_originalRegion = other.m_originalRegion;
    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_solver = nullptr;
    other.m_originalRegion = nullptr;
    other.m_elementList = nullptr;
}

SolverRegion& SolverRegion::operator=(SolverRegion&& other)
{
    if (&other == this) return *this;

    Region::operator=(std::move(other));

    m_solver = other.m_solver;
    m_originalRegion = other.m_originalRegion;
    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_solver = nullptr;
    other.m_originalRegion = nullptr;
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



SolverLine::SolverLine(Line &line, Solver*  const& solver)
    : SolverRegion(solver, line.getIndex(), &line), m_orientation(line.getOrientation())
{
    m_elementList = Region<std::pair<Tile*, Suggestions>>::make_line(m_orientation, m_index, solver->getSuggestions(), std::nullopt);
}

SolverLine::SolverLine(Solver*  const& solver, Orientation orientation, const TileValueType index)
    : SolverRegion(solver, index, &SolverLine::getLineRegionFrom(*solver->getGrid(), orientation, index)), m_orientation(orientation)
{
    m_elementList = Region<std::pair<Tile*, Suggestions>>::make_line(orientation, index, solver->getSuggestions(), std::nullopt);
}

SolverSubgrid::SolverSubgrid(Subgrid &subgrid, Solver*  const& solver)
    : SolverRegion(solver, subgrid.getIndex(), &subgrid)
{
    m_elementList = Region<std::pair<Tile*, Suggestions>>::make_subgrid(m_index, solver->getSuggestions(), std::nullopt);
}

SolverSubgrid::SolverSubgrid(Solver*  const& solver, const TileValueType index)
    : SolverRegion(solver, index, &solver->getGrid()->getSubgrid(index))
{
    m_elementList = Region<std::pair<Tile*, Suggestions>>::make_subgrid(index, solver->getSuggestions(), std::nullopt);
}

