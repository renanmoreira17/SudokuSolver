#ifndef __SUGGESTIONSQUANTITY_H__
#define __SUGGESTIONSQUANTITY_H__

#include "Util/GlobalDefinitions.hpp"

#include <unordered_map>
#include <vector>

class SuggestionsQuantity
{
  private:
    std::unordered_map<TileValueType, TileValueType> m_suggestionsQuan;

  public:
    SuggestionsQuantity();

    void addSuggestion(TileValueType value);
    void removeSuggestion(TileValueType value);
    TileValueType getSuggestionsQuantityFor(TileValueType value) const;

    std::vector<TileValueType> getSuggestionsWithQuantityGreaterThan(TileValueType value) const;
    std::vector<TileValueType> getSuggestionsWithQuantityLessThan(TileValueType value) const;
    std::vector<TileValueType> getSuggestionsWithQuantityEqualTo(TileValueType value) const;
    std::vector<TileValueType> getValidSuggestions() const;

  public:
    using SuggestionsQuan = std::unordered_map<TileValueType, TileValueType>;
    SuggestionsQuan::iterator begin() { return m_suggestionsQuan.begin(); }
    SuggestionsQuan::iterator end() { return m_suggestionsQuan.end(); }
    SuggestionsQuan::const_iterator cbegin() const { return m_suggestionsQuan.cbegin(); }
    SuggestionsQuan::const_iterator cend() const { return m_suggestionsQuan.cend(); }
    SuggestionsQuan::const_iterator begin() const { return cbegin(); }
    SuggestionsQuan::const_iterator end() const { return cend(); }
};

#endif // __SUGGESTIONSQUANTITY_H__