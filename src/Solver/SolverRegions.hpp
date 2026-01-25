#ifndef __SOLVERREGIONS_H__
#define __SOLVERREGIONS_H__

#include "SolverTypes.hpp"
#include "SuggestionsQuantity.hpp"
#include "Util/GlobalDefinitions.hpp"

#include <optional>
#include <unordered_set>
#include <vector>

class SolverTile;

enum class SolverRegionType
{
    LINE,
    SUBGRID
};

class SolverRegion
{
  public:
    SolverRegion() = delete;
    SolverRegion(SolverRegionType type,
                 RegionSpecificType specificType,
                 short index,
                 SolverTileVec tiles);
    virtual ~SolverRegion() = default;

    short getIndex() const { return m_index; }
    SolverRegionType getType() const { return m_type; }
    RegionSpecificType getRegionSpecificType() const { return m_specificType; }

    const SolverTileVec& getSolverTiles() const { return m_tiles; }

    const SuggestionsQuantity& getSuggestionsQuan() const { return m_suggestionsQuan; }

    void suggestionAdded(unsigned value);
    void suggestionRemoved(unsigned value);

    bool removeSuggestionsFromTiles(const std::vector<TileValueType>& values,
                                    const std::optional<SolverTileVec>& exceptFromTiles = std::nullopt);

    bool removeSingleSuggestionFromTiles(TileValueType values,
                                         const std::optional<SolverTileVec>& exceptFromTiles = std::nullopt)
    {
        return removeSuggestionsFromTiles({values}, exceptFromTiles);
    }

    SolverTileVec getTilesWithSuggestion(TileValueType value) const;
    SolverTileVec getTilesWithAllSuggestions(const std::vector<TileValueType>& suggestions) const;

    TileValueType getSuggestionsQuanFor(TileValueType value) const;

    SolverTileVec findLockedSetOfSuggestions(const std::unordered_set<TileValueType>& values) const;
    std::vector<SolverTileVec> findLockedSetsOfSize(unsigned short n) const;

    bool hasValue(TileValueType value) const;
    bool isCompleted() const;

    SolverTileVec::iterator begin() { return m_tiles.begin(); }
    SolverTileVec::iterator end() { return m_tiles.end(); }
    SolverTileVec::const_iterator begin() const { return m_tiles.begin(); }
    SolverTileVec::const_iterator end() const { return m_tiles.end(); }
    SolverTileVec::const_iterator cbegin() const { return m_tiles.cbegin(); }
    SolverTileVec::const_iterator cend() const { return m_tiles.cend(); }

  protected:
    SolverTileVec m_tiles;
    SuggestionsQuantity m_suggestionsQuan;

  private:
    SolverRegionType m_type;
    RegionSpecificType m_specificType;
    short m_index;
};

class SolverLine : public SolverRegion
{
  public:
    SolverLine(LineOrientation orientation, short index, SolverTileVec tiles);

    LineOrientation getLineOrientation() const { return m_orientation; }

  private:
    LineOrientation m_orientation;
};

class SolverSubgrid : public SolverRegion
{
  public:
    SolverSubgrid(short index, SolverTileVec tiles);
};

#endif // __SOLVERREGIONS_H__
