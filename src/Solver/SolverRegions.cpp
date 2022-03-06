#include "SolverRegions.hpp"

#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"

#include "Solver.hpp"
#include "SolverUtils.hpp"

#include <algorithm>
#include <exception>
#include <optional>
#include <utility>

SolverRegion::SolverRegion(Grid* grid, const unsigned index, const RegionType type)
    : Region(index, type, grid)
{}

SolverRegion::SolverRegion(SolverRegion&& other)
    : Region(std::move(other))
{
    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_elementList = nullptr;
}

Solver* SolverRegion::getSolver() const
{
    // try casting m_grid to Solver*, and return it if it is valid, otherwise throw an exception
    Solver* solver = dynamic_cast<Solver*>(getGrid());
    if (solver == nullptr)
    {
        throw std::runtime_error("SolverRegion::getSolver() - m_grid is not a Solver*");
    }
    return solver;
}

SolverRegion& SolverRegion::operator=(SolverRegion&& other)
{
    if (&other == this)
        return *this;

    Region::operator=(std::move(other));

    m_suggestionsQuan = std::move(other.m_suggestionsQuan);

    other.m_elementList = nullptr;

    return *this;
}

const SolverTileVec& SolverRegion::getSolverTiles() const
{
    if (m_solverTiles == nullptr)
    {
        m_solverTiles = std::make_unique<SolverTileVec>();
        for (const auto& tile : getTiles())
        {
            m_solverTiles->emplace_back(std::dynamic_pointer_cast<SolverTile>(tile));
        }
    }
    return *m_solverTiles;
}

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
    for (const SolverTilePtr& solverTile : getSolverTiles())
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
    for (const SolverTilePtr& solverTile : getSolverTiles())
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

    // retrieve all tiles with 2 <= x <= requested_size suggestions, that has at least one of the suggestions
    // in values
    SolverTileVec candidateTiles;
    std::copy_if(getSolverTiles().begin(),
                 getSolverTiles().end(),
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

// SPECIFIC REGIONS

SolverLine::SolverLine(Grid* grid, LineOrientation orientation, const short index)
    : Region(index, RegionType::LINE, grid)
    , SolverRegion(grid, index, RegionType::LINE)
    , Line(grid, orientation, index)
{}

SolverLine::SolverLine(const SolverLine& other, Grid* grid)
    : SolverLine(grid, other.getLineOrientation(), other.getIndex())
{
    m_suggestionsQuan = other.m_suggestionsQuan;
}

SolverSubgrid::SolverSubgrid(Grid* grid, const short index)
    : Region(index, RegionType::SUBGRID, grid)
    , SolverRegion(grid, index, RegionType::SUBGRID)
    , Subgrid(grid, index)
{}

SolverSubgrid::SolverSubgrid(const SolverSubgrid& other, Grid* grid)
    : SolverSubgrid(grid, other.getIndex())
{
    m_suggestionsQuan = other.m_suggestionsQuan;
}
