#ifndef __SOLVERREGIONS_H__
#define __SOLVERREGIONS_H__

#include "Board/ElementsContainer.hpp"
#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"

#include <map>
#include <optional>

class Solver;
class Line;
class Subgrid;
class SolverTile;

// key: suggestion value (1-9) - value: number of times this suggestion appears in the region
using SuggestionsQuan = std::map<TileValueType, TileValueType>;

class SolverRegion : virtual public Region
{
  private:
    Grid* m_grid;
    // Suggestions m_missing{1, 2, 3, 4, 5, 6, 7, 8, 9};
    SuggestionsQuan m_suggestionsQuan;

    mutable std::unique_ptr<std::vector<std::shared_ptr<SolverTile>>> m_solverTiles;

  public:
    SolverRegion() = delete;
    SolverRegion(Grid* grid, const unsigned index, RegionType type);
    SolverRegion(SolverRegion&& other);
    SolverRegion& operator=(SolverRegion&& other);
    virtual ~SolverRegion() = default;

    Solver* getSolver() const;

    const std::vector<std::shared_ptr<SolverTile>>& getSolverTiles() const;

    // Returns a map telling how many times each value is missing in this region
    const SuggestionsQuan& getSuggestionsQuan() const { return m_suggestionsQuan; }

    void suggestionAdded(const unsigned value);
    void suggestionRemoved(const unsigned value);

    bool removeSuggestionsFromTiles(const std::vector<TileValueType>& values,
                                    const std::optional<std::vector<std::shared_ptr<SolverTile>>>&
                                        exceptFromTiles = std::nullopt);

    bool removeSingleSuggestionFromTiles(
        TileValueType values,
        const std::optional<std::vector<std::shared_ptr<SolverTile>>>& exceptFromTiles =
            std::nullopt)
    {
        return removeSuggestionsFromTiles({values}, exceptFromTiles);
    }

    std::vector<std::shared_ptr<SolverTile>> getTilesWithSuggestion(TileValueType value) const;

    TileValueType getSuggestionsQuanFor(TileValueType value) const;
};

class SolverLine
    : public SolverRegion
    , public Line
{
  private:
  public:
    SolverLine(Grid* grid, LineOrientation orientation, const short index);
    SolverLine(SolverLine&& other) = default;
    SolverLine& operator=(SolverLine&& other) = default;
    ~SolverLine() {}
};

class SolverSubgrid
    : public SolverRegion
    , public Subgrid
{
  private:
  public:
    SolverSubgrid(Grid* grid, const short index);
    SolverSubgrid(SolverSubgrid&& other) = default;
    SolverSubgrid& operator=(SolverSubgrid&& other) = default;
    ~SolverSubgrid() {}
};

#endif // __SOLVERREGIONS_H__