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

using SuggestionsQuan = std::map<TileValueType, TileValueType>;

class SolverRegion : virtual public Region
{
  private:
    Grid* m_grid;
    // Suggestions m_missing{1, 2, 3, 4, 5, 6, 7, 8, 9};
    SuggestionsQuan m_suggestionsQuan;

  public:
    SolverRegion() = delete;
    SolverRegion(Grid* grid, const unsigned index, RegionType type);
    SolverRegion(const SolverRegion& other) = default;
    SolverRegion(SolverRegion&& other);
    SolverRegion& operator=(const SolverRegion& other) = default;
    SolverRegion& operator=(SolverRegion&& other);
    virtual ~SolverRegion() = default;

    Solver* getSolver() const;

    // Returns a map telling how many times each value is missing in this region
    const SuggestionsQuan& getSuggestionsQuan() const { return m_suggestionsQuan; }

    void suggestionAdded(const unsigned value);
    void suggestionRemoved(const unsigned value);

    bool removeSuggestionsFromTiles(
        const std::vector<TileValueType>& values,
        const std::optional<std::vector<std::shared_ptr<Tile>>>& exceptFromTiles = std::nullopt);

    bool removeSingleSuggestionFromTiles(
        TileValueType values,
        const std::optional<std::vector<std::shared_ptr<Tile>>>& exceptFromTiles = std::nullopt)
    {
        return removeSuggestionsFromTiles({values}, exceptFromTiles);
    }

    TileValueType getSuggestionsQuanFor(TileValueType value) const;
};

class SolverLine
    : public SolverRegion
    , public Line
{
  private:
  public:
    SolverLine(Grid* grid, LineOrientation orientation, const short index);
    SolverLine(const SolverLine& other) = default;
    SolverLine(SolverLine&& other) = default;
    SolverLine& operator=(const SolverLine& other) = default;
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
    SolverSubgrid(const SolverSubgrid& other) = default;
    SolverSubgrid(SolverSubgrid&& other) = default;
    SolverSubgrid& operator=(const SolverSubgrid& other) = default;
    SolverSubgrid& operator=(SolverSubgrid&& other) = default;
    ~SolverSubgrid() {}
};

#endif // __SOLVERREGIONS_H__