#include "NakedTriples.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverUtils.hpp"

#include <algorithm>

bool NakedTriples::analyze()
{
    for (const auto& region : m_solver.getAllRegions())
    {
        const auto& regionSuggestionsQuan = region->getSuggestionsQuan();
        const auto suggestionCount = std::count_if(
            regionSuggestionsQuan.begin(), regionSuggestionsQuan.end(), [](const auto& quanPair) {
                return quanPair.second == 3 || quanPair.second == 2;
            });
        if (suggestionCount >= 3)
        {
            return true;
        }
    }
    return false;
}

bool NakedTriples::perform()
{
    for (const auto& region : m_solver.getAllRegions())
    {
        const auto& regionSuggestionsQuan = region->getSuggestionsQuan();
        std::vector<std::shared_ptr<SolverTile>> tilesWith2Or3Suggestions;
        const auto& regionTiles = region->getSolverTiles();
        std::copy_if(regionTiles.begin(),
                     regionTiles.end(),
                     std::back_inserter(tilesWith2Or3Suggestions),
                     [](const std::shared_ptr<SolverTile>& tile) {
                         const auto suggestionCount = tile->getSuggestionsCount();
                         return suggestionCount == 2 || suggestionCount == 3;
                     });

        const SuggestionsQuantity suggestionsQuanForTiles =
            SolverUtils::collectSuggestionInformation(tilesWith2Or3Suggestions);

        if (tilesWith2Or3Suggestions.size() < 3)
        {
            continue;
        }

        for (auto tile1 = tilesWith2Or3Suggestions.begin();
             tile1 != std::prev(tilesWith2Or3Suggestions.end(), 2);
             ++tile1)
        {
            for (auto tile2 = tile1 + 1; tile2 != std::prev(tilesWith2Or3Suggestions.end(), 1);
                 ++tile2)
            {
                for (auto tile3 = tile2 + 1; tile3 != tilesWith2Or3Suggestions.end(); ++tile3)
                {
                    const std::vector<std::shared_ptr<SolverTile>> currentCombination = {
                        *tile1, *tile2, *tile3};
                    const auto currentCombinationSuggestionsQuan =
                        SolverUtils::collectSuggestionInformation(currentCombination);

                    const auto& validSuggestions = suggestionsQuanForTiles.getValidSuggestions();

                    if (validSuggestions.size() != 3)
                    {
                        continue;
                    }

                    bool isCombinationNakedTriple = true;
                    for (const auto& suggestion : validSuggestions)
                    {
                        const auto suggestionCount =
                            currentCombinationSuggestionsQuan.getSuggestionsQuantityFor(suggestion);
                        if (suggestionCount == 1)
                        {
                            isCombinationNakedTriple = false;
                            break;
                        }
                    }

                    if (!isCombinationNakedTriple)
                    {
                        continue;
                    }

                    const bool performed =
                        region->removeSuggestionsFromTiles(validSuggestions,
                                                           /*exceptFrom=*/currentCombination);

                    if (!performed)
                    {
                        continue;
                    }

                    m_solver.report("Naked Triples:\nA região {} possui 3 tiles, {}, {} e {}, em "
                                    "que as sugestões \"{}\" permutam-se entre si. Dessa forma, "
                                    "essas sugestões precisam ser a solução desses tiles e podem "
                                    "ser removidas de todos os outros.",
                                    *region,
                                    **tile1,
                                    **tile2,
                                    **tile3,
                                    joinContainer(validSuggestions, ", "));

                    return true;
                }
            }
        }
    }

    return false;
}
