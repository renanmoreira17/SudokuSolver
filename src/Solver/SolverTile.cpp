#include "SolverTile.hpp"

#include "SolverRegions.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>

SolverTile::SolverTile(TileValueType row, TileValueType col)
    : SolverTile(Coordinates{row, col})
{}

SolverTile::SolverTile(const Coordinates& coordinates, TileValueType value)
    : m_coordinates(coordinates)
    , m_value(value)
{}

void SolverTile::setValue(TileValueType value)
{
    m_value = value;

    const auto regions = getSolverRegions();
    for (const auto& suggestion : m_suggestions)
    {
        for (auto* region : regions)
        {
            if (region)
            {
                region->suggestionRemoved(suggestion);
            }
        }
    }
    m_suggestions.clear();

    for (auto* region : regions)
    {
        if (!region)
        {
            continue;
        }
        for (const auto& tile : region->getSolverTiles())
        {
            if (tile.get() == this || tile->hasValue())
            {
                continue;
            }
            tile->removeSuggestion(value);
        }
    }
}

TileValueType SolverTile::getValue() const
{
    return m_value;
}

bool SolverTile::hasValue() const
{
    return m_value != 0;
}

const Coordinates& SolverTile::getCoordinates() const
{
    return m_coordinates;
}

void SolverTile::computeSuggestions(const bool clear)
{
    if (clear)
    {
        const auto suggestionsCopy = m_suggestions;
        for (const auto& suggestion : suggestionsCopy)
        {
            removeSuggestion(suggestion);
        }
    }

    for (TileValueType value = 1; value <= 9; ++value)
    {
        if ((m_verticalLine && m_verticalLine->hasValue(value)) ||
            (m_horizontalLine && m_horizontalLine->hasValue(value)) ||
            (m_subgrid && m_subgrid->hasValue(value)))
        {
            continue;
        }
        addSuggestion(value);
    }
}

bool SolverTile::canPlaceValueInTile(const TileValueType value, const bool forceCheck) const
{
    if (forceCheck)
    {
        const auto regions = getSolverRegions();
        return std::none_of(
            regions.begin(),
            regions.end(),
            [=](auto* region) { return region != nullptr && region->hasValue(value); });
    }

    return m_suggestions.find(value) != m_suggestions.cend();
}

bool SolverTile::hasSuggestion(TileValueType value) const
{
    return m_suggestions.find(value) != m_suggestions.cend();
}

void SolverTile::addSuggestion(TileValueType value)
{
    const auto regions = getSolverRegions();
    const auto isInvalid =
        std::any_of(regions.begin(), regions.end(), [&](const SolverRegion* region) {
            return region != nullptr && region->hasValue(value);
        });
    if (isInvalid)
    {
        throw std::runtime_error(std::format(
            "Can't add suggestion {} to tile ({}, {}) because it's already in a region",
            value,
            m_coordinates.row,
            m_coordinates.col));
    }
    const auto insertedPair = m_suggestions.insert(value);
    if (insertedPair.second)
    {
        const auto solverRegions = getSolverRegions();
        for (const auto& solverRegion : solverRegions)
        {
            if (solverRegion)
            {
                solverRegion->suggestionAdded(value);
            }
        }
    }
}

bool SolverTile::removeSuggestion(TileValueType value)
{
    const auto erased = m_suggestions.erase(value);
    if (erased)
    {
        const auto regions = getSolverRegions();
        for (auto* solverRegion : regions)
        {
            if (solverRegion)
            {
                solverRegion->suggestionRemoved(value);
            }
        }
    }
    return erased;
}

bool SolverTile::removeSuggestions(const std::vector<TileValueType>& suggestions)
{
    bool changed = false;
    for (const auto& suggestion : suggestions)
    {
        changed |= removeSuggestion(suggestion);
    }
    return changed;
}

bool SolverTile::removeAllSuggestionsExceptFrom(const std::vector<TileValueType>& exceptionSuggestions)
{
    bool removed = false;
    const auto suggestionsCopy = getSuggestions();
    for (const auto& suggestion : suggestionsCopy)
    {
        if (std::find(exceptionSuggestions.begin(), exceptionSuggestions.end(), suggestion) ==
            exceptionSuggestions.cend())
        {
            removed |= removeSuggestion(suggestion);
        }
    }

    return removed;
}

unsigned short SolverTile::getSuggestionsCount() const
{
    return static_cast<unsigned short>(m_suggestions.size());
}

void SolverTile::setHorizontalLine(SolverLine* line)
{
    m_horizontalLine = line;
}

void SolverTile::setVerticalLine(SolverLine* line)
{
    m_verticalLine = line;
}

void SolverTile::setSubgrid(SolverSubgrid* subgrid)
{
    m_subgrid = subgrid;
}

std::vector<SolverRegion*> SolverTile::getSolverRegions() const
{
    return {m_horizontalLine, m_verticalLine, m_subgrid};
}

#ifdef DEBUG
#include "Util/UtilFunctions.hpp"

std::string SolverTile::toString() const
{
    std::stringstream ss;
    const auto& coordinates = getCoordinates();
    ss << std::format("Tile {}{}: ", convertRowToLetter(coordinates.row), coordinates.col + 1);
    if (hasValue())
    {
        ss << "Value: " << getValue() << " ";
    }
    else
    {
        ss << "Suggestions: ";
        for (const auto& suggestion : getSuggestions()) { ss << suggestion << " "; }
    }
    return ss.str();
}
#endif
