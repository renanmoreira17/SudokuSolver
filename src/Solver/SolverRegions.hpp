#ifndef __SOLVERREGIONS_H__
#define __SOLVERREGIONS_H__

#include "Board/ElementsContainer.hpp"
#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "SolverTypes.hpp"
#include "SuggestionsQuantity.hpp"

#include <map>
#include <optional>
#include <unordered_set>

class Solver;
class Line;
class Subgrid;
class SolverTile;

class SolverRegion : virtual public Region
{
  private:
    // Suggestions m_missing{1, 2, 3, 4, 5, 6, 7, 8, 9};
    mutable std::unique_ptr<SolverTileVec> m_solverTiles;

  protected:
    SuggestionsQuantity m_suggestionsQuan;

  public:
    SolverRegion() = delete;
    SolverRegion(Grid* grid, const unsigned index, RegionType type);
    SolverRegion(SolverRegion&& other);
    SolverRegion& operator=(SolverRegion&& other);
    virtual ~SolverRegion() = default;

    Solver* getSolver() const;

    const SolverTileVec& getSolverTiles() const;

    // Returns a map telling how many times each value is missing in this region
    const SuggestionsQuantity& getSuggestionsQuan() const { return m_suggestionsQuan; }

    void suggestionAdded(const unsigned value);
    void suggestionRemoved(const unsigned value);

    bool removeSuggestionsFromTiles(const std::vector<TileValueType>& values,
                                    const std::optional<SolverTileVec>& exceptFromTiles = std::nullopt);

    bool removeSingleSuggestionFromTiles(TileValueType values,
                                         const std::optional<SolverTileVec>& exceptFromTiles = std::nullopt)
    {
        return removeSuggestionsFromTiles({values}, exceptFromTiles);
    }

    SolverTileVec getTilesWithSuggestion(TileValueType value) const;

    TileValueType getSuggestionsQuanFor(TileValueType value) const;

    SolverTileVec findLockedSetOfSuggestions(const std::unordered_set<TileValueType>& values) const;
    std::vector<SolverTileVec> findLockedSetsOfSize(unsigned short n) const;
};

class SolverLine
    : public SolverRegion
    , public Line
{
  private:
  public:
    SolverLine(Grid* grid, LineOrientation orientation, const short index);
    SolverLine(const SolverLine& other, Grid* grid);
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
    SolverSubgrid(const SolverSubgrid& other, Grid* grid);
    SolverSubgrid(SolverSubgrid&& other) = default;
    SolverSubgrid& operator=(SolverSubgrid&& other) = default;
    ~SolverSubgrid() {}
};

#endif // __SOLVERREGIONS_H__