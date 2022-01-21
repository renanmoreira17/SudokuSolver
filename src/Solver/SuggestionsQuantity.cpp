#include "SuggestionsQuantity.hpp"

SuggestionsQuantity::SuggestionsQuantity()
{
    for (TileValueType i = 1; i <= 9; i++) { m_suggestionsQuan[i] = 0; }
}

void SuggestionsQuantity::addSuggestion(TileValueType value)
{
    if (value < 1 || value > 9)
    {
        throw std::invalid_argument("Value must be between 1 and 9");
    }
    const auto currentValue = m_suggestionsQuan[value];
    if (currentValue == 9)
    {
        throw std::out_of_range("Trying to increase suggestions number, but it's already at 9!");
    }
    m_suggestionsQuan[value] = currentValue + 1;
}

void SuggestionsQuantity::removeSuggestion(TileValueType value)
{
    if (value < 1 || value > 9)
    {
        throw std::invalid_argument("Value must be between 1 and 9");
    }
    const auto currentValue = m_suggestionsQuan[value];
    if (currentValue == 0)
    {
        throw std::out_of_range("Trying to decrease suggestions number, but it's already at 0!");
    }
    m_suggestionsQuan[value] = currentValue - 1;
}

TileValueType SuggestionsQuantity::getSuggestionsQuantityFor(TileValueType value) const
{
    if (value < 1 || value > 9)
    {
        throw std::out_of_range(
            "Trying to get suggestions quantity for a value smaller than 1 or greater than 9!");
    }
    return m_suggestionsQuan.at(value);
}

std::vector<TileValueType>
SuggestionsQuantity::getSuggestionsWithQuantityGreaterThan(TileValueType value) const
{
    std::vector<TileValueType> suggestions;
    for (const auto& suggestionPair : m_suggestionsQuan)
    {
        if (suggestionPair.second > value)
        {
            suggestions.push_back(suggestionPair.first);
        }
    }
    return suggestions;
}
std::vector<TileValueType>
SuggestionsQuantity::getSuggestionsWithQuantityLessThan(TileValueType value) const
{
    std::vector<TileValueType> suggestions;
    for (const auto& suggestionPair : m_suggestionsQuan)
    {
        if (suggestionPair.second < value)
        {
            suggestions.push_back(suggestionPair.first);
        }
    }
    return suggestions;
}
std::vector<TileValueType>
SuggestionsQuantity::getSuggestionsWithQuantityEqualTo(TileValueType value) const
{
    std::vector<TileValueType> suggestions;
    for (const auto& suggestionPair : m_suggestionsQuan)
    {
        if (suggestionPair.second == value)
        {
            suggestions.push_back(suggestionPair.first);
        }
    }
    return suggestions;
}
std::vector<TileValueType> SuggestionsQuantity::getValidSuggestions() const
{
    return getSuggestionsWithQuantityGreaterThan(0);
}
