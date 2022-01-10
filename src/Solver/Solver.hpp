#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "Board/ElementsContainer.hpp"
#include "Board/Grid.hpp"
#include "Board/GridPrinter.hpp"
#include "Reporter.hpp"
#include "SolverRegions.hpp"
#include "Techniques/Singles.hpp"

#include <array>
#include <memory>
#include <vector>

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
    std::array<std::shared_ptr<SolverRegion>, 27> m_allRegions;

    std::vector<std::unique_ptr<Technique>> m_techniques{};

    void initialize();

    std::shared_ptr<Reporter> m_reporter;

  protected:
    std::vector<std::string>
    requestTileDisplayStringForCoordinate(const TileValueType row,
                                          const TileValueType col) const override;

  public:
    const std::array<SolverRegion* const, 3> getSolverRegions(const Tile& tile);
    const std::array<const SolverRegion* const, 3> getSolverRegions(const Tile& tile) const;

    std::array<std::shared_ptr<SolverRegion>, 27>& getAllRegions() { return m_allRegions; }
    const std::array<std::shared_ptr<SolverRegion>, 27>& getAllRegions() const
    {
        return m_allRegions;
    }

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
    ~Solver() = default;

    void solve();
};

#endif // __SOLVER_H__