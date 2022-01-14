#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "Board/Grid.hpp"
#include "Reporter.hpp"

#include <array>
#include <memory>
#include <vector>

class Technique;

class SolverComponentsContructor : public ComponentsConstructor
{
  public:
    std::shared_ptr<Tile> createTile(Grid* grid, TileValueType row, TileValueType col) override;
    std::shared_ptr<Line> createLine(Grid* grid, LineOrientation row, short index) override;
    std::shared_ptr<Subgrid> createSubgrid(Grid* grid, short index) override;
};

class Solver : public Grid
{
  private:
    mutable std::vector<std::shared_ptr<SolverRegion>> m_allSolverRegions;

    mutable std::vector<std::shared_ptr<SolverSubgrid>> m_allSolverSubgrids;
    mutable std::vector<std::shared_ptr<SolverLine>> m_allSolverHorizontalLines;
    mutable std::vector<std::shared_ptr<SolverLine>> m_allSolverVerticalLines;

    std::vector<std::unique_ptr<Technique>> m_techniques;

    void initialize();
    void initializeTechniques();

    std::shared_ptr<Reporter> m_reporter;

  protected:
    std::vector<std::string>
    requestTileDisplayStringForCoordinate(const TileValueType row,
                                          const TileValueType col) const override;

  public:
    const std::array<SolverRegion* const, 3> getSolverRegions(const Tile& tile);
    const std::array<const SolverRegion* const, 3> getSolverRegions(const Tile& tile) const;

    const std::vector<std::shared_ptr<SolverRegion>>& getAllRegions() const;
    const std::vector<std::shared_ptr<SolverSubgrid>>& getAllSolverSubgrids() const;
    const std::vector<std::shared_ptr<SolverLine>>& getAllSolverHorizontalLines() const;
    const std::vector<std::shared_ptr<SolverLine>>& getAllSolverVerticalLines() const;

    void computeAllSuggestions(const bool clear = false);
    void computeTileSuggestions(const Tile& tile, const bool clear = false);

    bool canPlaceValueInTile(const Tile& tile,
                             const TileValueType value,
                             const bool forceCheck = false) const;

    friend int main();

    void setReporter(const std::shared_ptr<Reporter>& reporter) { m_reporter = reporter; }

    template<typename FormatString, typename... Args>
    void report(FormatString formatString, Args&&... args) const
    {
        if (m_reporter)
        {
            m_reporter->report(formatString, std::forward<Args>(args)...);
            printGrid();
        }
    }

  public:
    Solver();
    Solver(const std::string& fromBoard);
    ~Solver();

    void solve();
};

#endif // __SOLVER_H__