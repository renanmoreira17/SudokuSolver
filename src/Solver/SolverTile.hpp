#ifndef __SOLVERTILE_H__
#define __SOLVERTILE_H__

#include "Board/Tile.hpp"
#include "SolverTypes.hpp"
#include "Util/GlobalDefinitions.hpp"

#include <string>
#include <unordered_set>
#include <vector>

class Solver;
class SolverLine;
class SolverSubgrid;
class SolverRegion;

using Suggestions = std::unordered_set<TileValueType>;

class SolverTile : public Tile
{
  private:
    Suggestions m_suggestions;

  public:
    SolverTile(Grid* grid, TileValueType row, TileValueType col);
    SolverTile(Grid* grid, const Coordinates& coordinates, TileValueType value = 0);
    SolverTile(const SolverTile& other, Grid* grid);
    ~SolverTile() = default;

    void setValue(TileValueType value) override;

    void computeSuggestions(bool clear = false);

    const Suggestions& getSuggestions() const { return m_suggestions; };
    bool hasSuggestion(TileValueType value) const;
    void addSuggestion(TileValueType value);
    bool removeSuggestion(TileValueType value);
    bool removeSuggestions(const std::vector<TileValueType>& suggestions);
    bool removeAllSuggestionsExceptFrom(const std::vector<TileValueType>& exceptionSuggestions);
    unsigned short getSuggestionsCount() const;

    SolverLine* getSolverHorizontalLine() const;
    SolverLine* getSolverVerticalLine() const;
    SolverSubgrid* getSolverSubgrid() const;

    std::vector<SolverRegion*> getSolverRegions() const;

    bool canPlaceValueInTile(TileValueType value, bool forceCheck = false) const;

#ifdef DEBUG
    std::string toString() const;
#endif
};

#endif // __SOLVERTILE_H__