#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "Board/Grid.hpp"
#include "Board/ElementsContainer.hpp"
#include "SolverRegions.hpp"
#include <set>
#include <tuple>
#include <array>
#include <vector>
#include <utility>
#include <memory>
#include "Techniques/Singles.hpp"

class Solver
{
private:
    Grid *m_grid;
    GridElements<std::pair<Tile*, Suggestions>> m_suggestions;

    SolverLine m_horizontalLines[9];
    SolverLine m_verticalLines[9];
    SolverSubgrid m_subgrids[9];
    std::array<SolverRegion*, 27> m_allRegions;

    std::vector<std::unique_ptr<Technique>> m_techniques{};

    void initializeTechniques();
public:
    Grid* const& getGrid() { return m_grid; }
    const Grid* const& getGrid() const { return m_grid; }

    std::set<TileValueType> &getSuggestions(TileValueType row, TileValueType col);
    std::set<TileValueType> &getSuggestions(const Tile &tile);
    const std::set<TileValueType> &getSuggestions(TileValueType row, TileValueType col) const;
    const std::set<TileValueType> &getSuggestions(const Tile &tile) const;

    SolverLine& getSolverHorizontalLine(const Tile& tile);
    const SolverLine& getSolverHorizontalLine(const Tile& tile) const;
    SolverLine& getSolverVerticalLine(const Tile& tile);
    const SolverLine& getSolverVerticalLine(const Tile& tile) const;
    SolverSubgrid& getSolverSubgrid(const Tile& tile);
    const SolverSubgrid& getSolverSubgrid(const Tile& tile) const;
    const std::tuple<SolverLine&, SolverLine&, SolverSubgrid&> getSolverRegionsTuple(const Tile& tile);
    const std::tuple<const SolverLine&, const SolverLine&, const SolverSubgrid&> getSolverRegionsTuple(const Tile& tile) const;
    const std::array<SolverRegion* const, 3> getSolverRegions(const Tile& tile);
    const std::array<const SolverRegion* const, 3> getSolverRegions(const Tile& tile) const;

    std::array<SolverRegion*, 27>& getAllRegions() { return m_allRegions; }
    const std::array<SolverRegion*, 27>& getAllRegions() const { return m_allRegions; }

    void computeAllSuggestions(const bool clear = false);
    void computeTileSuggestions(const Tile& tile, const bool clear = false);

    bool canPlaceValueInTile(const Tile& tile, const TileValueType value, const bool forceCheck = false) const;

    void placeValueInTile(Tile& tile, const TileValueType value);
    void placeValueInTile(const Tile& tile, const TileValueType value);
    void placeValueInTile(Tile* const& tile, const TileValueType value);
    void placeValueInTile(const Tile* const& tile, const TileValueType value);

    friend int main();

public:
    Solver() = default;
    Solver(Grid *const &grid);
    Solver(Grid& grid) : Solver(&grid) {}
    ~Solver() = default;

    void solve();

    GridElements<std::pair<Tile*, Suggestions>>& getSuggestions() { return m_suggestions; }
    const GridElements<std::pair<Tile*, Suggestions>>& getSuggestions() const { return m_suggestions; }
};

#endif // __SOLVER_H__