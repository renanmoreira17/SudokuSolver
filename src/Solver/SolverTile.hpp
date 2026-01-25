#ifndef __SOLVERTILE_H__
#define __SOLVERTILE_H__

#include "SolverTypes.hpp"
#include "Util/GlobalDefinitions.hpp"

#include <string>
#include <unordered_set>
#include <vector>

class SolverLine;
class SolverSubgrid;
class SolverRegion;

using Suggestions = std::unordered_set<TileValueType>;

class SolverTile
{
  public:
    SolverTile(TileValueType row, TileValueType col);
    SolverTile(const Coordinates& coordinates, TileValueType value = 0);
    SolverTile(const SolverTile& other) = default;
    SolverTile& operator=(const SolverTile& other) = default;
    ~SolverTile() = default;

    void setValue(TileValueType value);
    TileValueType getValue() const;
    bool hasValue() const;

    const Coordinates& getCoordinates() const;

    void computeSuggestions(bool clear = false);

    const Suggestions& getSuggestions() const { return m_suggestions; };
    bool hasSuggestion(TileValueType value) const;
    void addSuggestion(TileValueType value);
    bool removeSuggestion(TileValueType value);
    bool removeSuggestions(const std::vector<TileValueType>& suggestions);
    bool removeAllSuggestionsExceptFrom(const std::vector<TileValueType>& exceptionSuggestions);
    unsigned short getSuggestionsCount() const;

    void setHorizontalLine(SolverLine* line);
    void setVerticalLine(SolverLine* line);
    void setSubgrid(SolverSubgrid* subgrid);

    SolverLine* getSolverHorizontalLine() const { return m_horizontalLine; }
    SolverLine* getSolverVerticalLine() const { return m_verticalLine; }
    SolverSubgrid* getSolverSubgrid() const { return m_subgrid; }

    std::vector<SolverRegion*> getSolverRegions() const;

    bool canPlaceValueInTile(TileValueType value, bool forceCheck = false) const;

#ifdef DEBUG
    std::string toString() const;
#endif

  private:
    Coordinates m_coordinates;
    TileValueType m_value{0};
    Suggestions m_suggestions;

    SolverLine* m_horizontalLine{nullptr};
    SolverLine* m_verticalLine{nullptr};
    SolverSubgrid* m_subgrid{nullptr};
};

#endif // __SOLVERTILE_H__
