#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "Reporter.hpp"
#include "SolverRegions.hpp"
#include "SolverTile.hpp"
#include "SolverTypes.hpp"

#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Technique;

class Solver
{
  private:
    SolverTileVec m_tiles;
    std::vector<std::shared_ptr<SolverSubgrid>> m_subgrids;
    std::vector<std::shared_ptr<SolverLine>> m_horizontalLines;
    std::vector<std::shared_ptr<SolverLine>> m_verticalLines;
    std::vector<std::shared_ptr<SolverRegion>> m_allRegions;

    std::vector<std::unique_ptr<Technique>> m_techniques;
    std::shared_ptr<Reporter> m_reporter;

    bool m_initializedWithSuggestions{false};

    void initializeLayout();
    void initializeTechniques();
    void initializeFromBoard(const std::string& fromBoard);

    SolverTilePtr getTileAt(TileValueType row, TileValueType col) const;
    std::vector<std::string> requestTileDisplayStringForCoordinate(TileValueType row,
                                                                   TileValueType col) const;

  public:
    Solver();
    explicit Solver(const std::string& fromBoard);
    ~Solver();

    SolverTileVec& getGridTiles() { return m_tiles; }
    const SolverTileVec& getGridTiles() const { return m_tiles; }

    SolverTilePtr operator()(TileValueType row, TileValueType col) const;
    SolverTilePtr operator()(const Coordinates& coordinates) const;

    const std::vector<std::shared_ptr<SolverRegion>>& getAllRegions() const;
    const std::vector<std::shared_ptr<SolverSubgrid>>& getAllSolverSubgrids() const;
    const std::vector<std::shared_ptr<SolverLine>>& getAllSolverHorizontalLines() const;
    const std::vector<std::shared_ptr<SolverLine>>& getAllSolverVerticalLines() const;

    void computeAllSuggestions(bool clear = false);
    void computeTileSuggestions(const SolverTilePtr& tile, bool clear = false);

    bool canPlaceValueInTile(const SolverTile& tile, TileValueType value, bool forceCheck = false) const;

    bool isSolved() const;

    std::string getBoardString() const;
    void printGrid() const;

    void setReporter(const std::shared_ptr<Reporter>& reporter) { m_reporter = reporter; }

    template<typename... Args>
    void report(const std::format_string<Args...>& fmt, Args&&... args) const
    {
        if (m_reporter)
        {
            m_reporter->report(fmt, std::forward<Args>(args)...);
            printGrid();
        }
    }

    void solve();
};

#endif // __SOLVER_H__
