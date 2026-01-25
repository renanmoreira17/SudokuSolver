#include "SolverRegions.hpp"

#include "SolverTile.hpp"
#include "SolverUtils.hpp"

#include <algorithm>
#include <utility>

SolverRegion::SolverRegion(SolverRegionType type,
                           RegionSpecificType specificType,
                           short index,
                           SolverTileVec tiles)
    : m_tiles(std::move(tiles))
    , m_type(type)
    , m_specificType(specificType)
    , m_index(index)
{}

void SolverRegion::suggestionAdded(const unsigned value)
{
    m_suggestionsQuan.addSuggestion(value);
}

void SolverRegion::suggestionRemoved(const unsigned value)
{
    m_suggestionsQuan.removeSuggestion(value);
}

TileValueType SolverRegion::getSuggestionsQuanFor(TileValueType value) const
{
    return m_suggestionsQuan.getSuggestionsQuantityFor(value);
}

bool SolverRegion::removeSuggestionsFromTiles(const std::vector<TileValueType>& values,
                                              const std::optional<SolverTileVec>& exceptFromTiles)
{
    bool performed = false;
    for (const SolverTilePtr& solverTile : m_tiles)
    {
        if (solverTile->getSuggestions().empty() ||
            (exceptFromTiles.has_value() &&
             std::any_of(exceptFromTiles->begin(),
                         exceptFromTiles->end(),
                         [&solverTile](const SolverTilePtr& t) { return t == solverTile; })))
        {
            continue;
        }

        for (const auto& currentSuggestion : values)
        {
            const bool erased = solverTile->removeSuggestion(currentSuggestion);
            performed |= erased;
        }
    }

    return performed;
}

SolverTileVec SolverRegion::getTilesWithSuggestion(TileValueType value) const
{
    return getTilesWithAllSuggestions({value});
}

SolverTileVec SolverRegion::getTilesWithAllSuggestions(const std::vector<TileValueType>& suggestions) const
{
    SolverTileVec tiles;
    for (const SolverTilePtr& solverTile : m_tiles)
    {
        if (solverTile->getSuggestions().empty())
            continue;

        if (std::all_of(suggestions.begin(), suggestions.end(), [&solverTile](const TileValueType& s) {
                return solverTile->hasSuggestion(s);
            }))
        {
            tiles.emplace_back(solverTile);
        }
    }
    return tiles;
}

SolverTileVec SolverRegion::findLockedSetOfSuggestions(const std::unordered_set<TileValueType>& values) const
{
    const auto requestedSize = values.size();
    if (requestedSize <= 1 || requestedSize > 4)
    {
        return {};
    }

    SolverTileVec candidateTiles;
    std::copy_if(m_tiles.begin(),
                 m_tiles.end(),
                 std::back_inserter(candidateTiles),
                 [&](const SolverTilePtr& tile) {
                     const auto suggestionCount = tile->getSuggestionsCount();
                     return suggestionCount >= 2 && suggestionCount <= requestedSize &&
                            std::any_of(values.begin(), values.end(), [&](TileValueType suggestion) {
                                return tile->hasSuggestion(suggestion);
                            });
                 });

    if (candidateTiles.size() < requestedSize)
    {
        return {};
    }

    const std::vector<SolverTileVec> combinations =
        SolverUtils::createCombination(requestedSize, candidateTiles);

    for (const auto& combination : combinations)
    {
        const auto combinationSuggestionsQuan = SolverUtils::collectSuggestionInformation(combination);
        const auto validSuggestions = combinationSuggestionsQuan.getValidSuggestions();
        if (validSuggestions.size() != requestedSize)
        {
            continue;
        }
        bool shouldSkip = false;
        for (const auto& value : values)
        {
            const auto quan = combinationSuggestionsQuan.getSuggestionsQuantityFor(value);
            if (quan < 2)
            {
                shouldSkip = true;
                break;
            }
        }
        if (shouldSkip)
        {
            continue;
        }
        const bool isFreeOfSingleSuggestion =
            combinationSuggestionsQuan.getSuggestionsWithQuantityEqualTo(1).empty();
        if (isFreeOfSingleSuggestion)
        {
            return combination;
        }
    }

    return {};
}

std::vector<SolverTileVec> SolverRegion::findLockedSetsOfSize(const unsigned short n) const
{
    const auto& suggestionsQuan = getSuggestionsQuan();
    const auto targetSuggestions = suggestionsQuan.getSuggestionsWithQuantityGreaterThan(1);
    if (targetSuggestions.size() < n)
    {
        return std::vector<SolverTileVec>();
    }

    std::vector<SolverTileVec> result;

    const auto combinations = SolverUtils::createCombination(n, targetSuggestions);
    for (const auto& combination : combinations)
    {
        auto lockedSet =
            findLockedSetOfSuggestions(std::unordered_set(combination.begin(), combination.end()));
        if (!lockedSet.empty())
        {
            result.emplace_back(std::move(lockedSet));
        }
    }

    return result;
}

bool SolverRegion::hasValue(TileValueType value) const
{
    return std::any_of(m_tiles.begin(), m_tiles.end(), [&](const SolverTilePtr& tile) {
        return tile->hasValue() && tile->getValue() == value;
    });
}

bool SolverRegion::isCompleted() const
{
    return std::all_of(m_tiles.begin(),
                       m_tiles.end(),
                       [](const SolverTilePtr& tile) { return tile->hasValue(); });
}

SolverLine::SolverLine(LineOrientation orientation, short index, SolverTileVec tiles)
    : SolverRegion(SolverRegionType::LINE,
                   orientation == LineOrientation::HORIZONTAL ? RegionSpecificType::HORIZONTAL_LINE
                                                              : RegionSpecificType::VERTICAL_LINE,
                   index,
                   std::move(tiles))
    , m_orientation(orientation)
{}

SolverSubgrid::SolverSubgrid(short index, SolverTileVec tiles)
    : SolverRegion(SolverRegionType::SUBGRID, RegionSpecificType::SUBGRID, index, std::move(tiles))
{}
