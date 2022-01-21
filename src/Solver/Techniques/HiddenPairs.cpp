#include "HiddenPairs.hpp"
#include "Solver/Solver.hpp"
#include "Util/UtilFunctions.hpp"

#include <fmt/format.h>

bool HiddenPairs::analyze()
{
    for (const auto& region : m_solver.getAllRegions())
    {
        if (region->isCompleted())
        {
            continue;
        }

        short numberOfSuggestionsThatAppear2Times = 0;
        for (const auto& suggestionPair : region->getSuggestionsQuan())
        {
            if (suggestionPair.second == 2)
            {
                ++numberOfSuggestionsThatAppear2Times;
                // if at least 2 suggestions appear 2 times in the region, then
                // we can proceed the analysis
                if (numberOfSuggestionsThatAppear2Times == 2)
                {
                    break;
                }
            }
        }

        // there must be at least 2 suggestions that appear 2 times
        // otherwise there is no point in checking for hidden pairs
        if (numberOfSuggestionsThatAppear2Times < 2)
        {
            continue;
        }

        short numberOfSuggestionsOfTiles = 0;
        for (const auto& tile : region->getSolverTiles())
        {
            if (tile->getSuggestions().size() >= 2)
            {
                ++numberOfSuggestionsOfTiles;
                if (numberOfSuggestionsOfTiles == 2)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool HiddenPairs::perform()
{
    bool performed = false;
    for (const auto& region : m_solver.getAllRegions())
    {
        if (region->isCompleted())
        {
            continue;
        }

        const SuggestionsQuantity& suggestionQuan = region->getSuggestionsQuan();
        // suggestions with exactly 2 appearances
        std::vector<TileValueType> suggestionsWith2Appearances;
        for (const auto& suggestionPair : suggestionQuan)
        {
            if (suggestionPair.second == 2)
            {
                suggestionsWith2Appearances.push_back(suggestionPair.first);
            }
        }

        // filter tiles that have suggestions present in suggestionsWith2Appearances and
        // at least 2 suggestions
        std::vector<std::shared_ptr<SolverTile>> solverTilesWithAtLeast2DesiredSuggestions;
        std::copy_if(region->getSolverTiles().begin(),
                     region->getSolverTiles().end(),
                     std::back_inserter(solverTilesWithAtLeast2DesiredSuggestions),
                     [&suggestionsWith2Appearances](const std::shared_ptr<SolverTile>& solverTile) {
                         if (solverTile->getSuggestionsCount() < 2)
                         {
                             return false;
                         }
                         unsigned short matchedSuggestions = 0;
                         for (const auto& desiredSuggestion : suggestionsWith2Appearances)
                         {
                             if (solverTile->hasSuggestion(desiredSuggestion))
                             {
                                 ++matchedSuggestions;
                                 if (matchedSuggestions == 2)
                                 {
                                     return true;
                                 }
                             }
                         }
                         return false;
                     });

        if (solverTilesWithAtLeast2DesiredSuggestions.size() < 2)
        {
            continue;
        }

        const auto& suggestionIntersection =
            [&suggestionsWith2Appearances](const std::shared_ptr<SolverTile>& solverTile1,
                                           const std::shared_ptr<SolverTile>& solverTile2) {
                std::vector<TileValueType> intersection;
                for (const auto& suggestion : solverTile1->getSuggestions())
                {
                    if (std::find(suggestionsWith2Appearances.begin(),
                                  suggestionsWith2Appearances.end(),
                                  suggestion) == suggestionsWith2Appearances.end())
                    {
                        continue;
                    }
                    if (solverTile2->hasSuggestion(suggestion))
                    {
                        intersection.push_back(suggestion);
                    }
                }
                return intersection;
            };

        for (auto currentSolverTileIt = solverTilesWithAtLeast2DesiredSuggestions.begin();
             currentSolverTileIt != solverTilesWithAtLeast2DesiredSuggestions.end() - 1;
             ++currentSolverTileIt)
        {
            bool breakLoop = false;
            for (auto otherSolverTileIt = currentSolverTileIt + 1;
                 otherSolverTileIt != solverTilesWithAtLeast2DesiredSuggestions.end();
                 ++otherSolverTileIt)
            {
                const auto& intersection =
                    suggestionIntersection(*currentSolverTileIt, *otherSolverTileIt);
                // if there were 2 equal suggestions in this iteration, it means that they
                // are the hidden pairs
                // this is because the suggestions used to filter the tiles only appear
                // twice. If we manage to find two tiles with the same two suggestions
                // from those filtered, then it means that they are the only ones with
                // those suggestions and they are the hidden pairs
                if (intersection.size() == 2)
                {
                    auto& solverTile1 = *currentSolverTileIt;
                    auto& solverTile2 = *otherSolverTileIt;
                    // if both tiles have only 2 suggestions, then there are no extra suggestions
                    // to remove
                    if (solverTile1->getSuggestionsCount() == 2 &&
                        solverTile2->getSuggestionsCount() == 2)
                    {
                        continue;
                    }

                    solverTile1->removeAllSuggestionsExceptFrom(intersection);
                    solverTile2->removeAllSuggestionsExceptFrom(intersection);

                    performed = true;

                    m_solver.report(
                        "Hidden Pairs:\nA região {} possui dois Tiles, {} e {}, em que as "
                        "sugestões {} aparecem apenas neles. Dessa forma, o valor de um Tile deve "
                        "ser uma dessas sugestões e o outro Tile a outra sugestão. Logo, todas as "
                        "outras sugestões desses Tiles podem ser removidas.",
                        *region,
                        *solverTile1,
                        *solverTile2,
                        joinContainer(intersection, ","));

                    breakLoop = true;
                    break;
                }
            }
            if (breakLoop)
            {
                break;
            }
        }
    }

    return performed;
}