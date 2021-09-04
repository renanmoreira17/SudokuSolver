#ifndef __SOLVERREGIONS_H__
#define __SOLVERREGIONS_H__

#include "Board/Region.hpp"
#include "Board/ElementsContainer.hpp"
#include "Board/Grid.hpp"
#include <set>
#include <map>

class Solver;
class Line;
class Subgrid;

using SuggestionsQuan = std::map<TileValueType, TileValueType>;
using Suggestions = std::set<TileValueType>;


class SolverRegion : public Region<std::pair<Tile*, Suggestions>>
{
private:
    Solver* m_solver;
    // Suggestions m_missing{1, 2, 3, 4, 5, 6, 7, 8, 9};
    SuggestionsQuan m_suggestionsQuan;
    Region<Tile>* m_originalRegion;
public:
    SolverRegion(Solver* const& solver, const unsigned index, Region<Tile>* const& originalRegion);
    SolverRegion() = default;
    SolverRegion(const SolverRegion& other)             = default;
    SolverRegion(SolverRegion&& other);
    SolverRegion& operator=(const SolverRegion& other)  = default;
    SolverRegion& operator=(SolverRegion&& other);
    ~SolverRegion() {}

    Solver* const& getSolver() { return m_solver; }
    const Solver* const& getSolver() const { return m_solver; }
    SuggestionsQuan& getSuggestionsQuan() { return m_suggestionsQuan; }
    const SuggestionsQuan& getSuggestionsQuan() const { return m_suggestionsQuan; }
    void suggestionAdded(const unsigned value);
    void suggestionRemoved(const unsigned value);
    TileValueType getSuggestionsQuanFor(TileValueType value) const;

    Region<Tile>& getOriginalRegion() { return *m_originalRegion; }
    const Region<Tile>& getOriginalRegion() const { return *m_originalRegion; }
};


class SolverLine : public SolverRegion
{
private:
    Orientation m_orientation;
    static Region<Tile>& getLineRegionFrom(Grid& grid, const Orientation& orientation, const TileValueType index)
    {
        if (orientation == horizontal)
        {
            return grid.getHorizontalLine(index);
        }
        else
        {
            return grid.getVerticalLine(index);
        }
    }
public:
    SolverLine(Line& line, Solver* const& solver);
    SolverLine(Solver* const& solver, Orientation orientation, const TileValueType index);
    SolverLine()                                    = default;
    SolverLine(const SolverLine& other)             = default;
    SolverLine(SolverLine&& other)                  = default;
    SolverLine& operator=(const SolverLine& other)  = default;
    SolverLine& operator=(SolverLine&& other)       = default;
    ~SolverLine() {}
};

class SolverSubgrid : public SolverRegion
{
private:
public:
    SolverSubgrid(Subgrid& subgrid, Solver* const& solver);
    SolverSubgrid(Solver* const& solver, const TileValueType index);
    SolverSubgrid()                                       = default;
    SolverSubgrid(const SolverSubgrid& other)             = default;
    SolverSubgrid(SolverSubgrid&& other)                  = default;
    SolverSubgrid& operator=(const SolverSubgrid& other)  = default;
    SolverSubgrid& operator=(SolverSubgrid&& other)       = default;
    ~SolverSubgrid() {}
};


#endif // __SOLVERREGIONS_H__