#include "NakedPairs.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"

#include <list>
#include <memory>
#include <type_traits>
#include <utility>

bool NakedPairs::analyze()
{
    for (auto&& region : m_solver.getAllRegions())
    {
        short suggestionQuan = 0;
        for (auto&& tile : *region)
        {
            // cast tile to SolverTilePtr, check if the number of suggestions is
            // greater than 2, if so, increments suggestionQuan, and if suggestionQuan is greater
            // than 2, then return true
            if (std::dynamic_pointer_cast<SolverTile>(tile)->getSuggestions().size() >= 2)
            {
                ++suggestionQuan;
                if (suggestionQuan == 2)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool NakedPairs::perform()
{
    bool performed = false;
    for (const auto& region : m_solver.getAllRegions())
    {
        if (region->isCompleted())
            continue;

        std::list<const SolverTilePtr> solverTilesWith2Suggestions;

        // filtra todos tiles que tem a quantidade de sugestões == 2
        for (const auto& tile : *region)
        {
            const auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
            if (solverTile->getSuggestions().size() == 2)
            {
                solverTilesWith2Suggestions.push_back(solverTile);
            }
        }

        if (solverTilesWith2Suggestions.size() < 2)
            continue;

        auto it = solverTilesWith2Suggestions.begin();
        // so pode ser feito uma vez por region, por mais que haja outros hidden pairs na region.
        // isso porque a remoçao de sugestoes de uma region pode interfereir a filteredSuggestions
        // se houver remoçao de uma das sugestoes dos possiveis hidden pairs
        for (const auto& currentSolverTileWith2Suggestions : solverTilesWith2Suggestions)
        {
            // procura, no restante da lista, se há algum tile que possui a mesma sugestão
            const auto foundSolverTileWithEqualSuggestions = std::find_if(
                solverTilesWith2Suggestions.begin(),
                solverTilesWith2Suggestions.end(),
                [&](const SolverTilePtr& iteratedSolverTileWith2Suggestions) -> bool {
                    return (iteratedSolverTileWith2Suggestions != currentSolverTileWith2Suggestions) &&
                           (iteratedSolverTileWith2Suggestions->getSuggestions() ==
                            currentSolverTileWith2Suggestions->getSuggestions());
                });

            if (foundSolverTileWithEqualSuggestions != solverTilesWith2Suggestions.cend())
            {
                // e remove as sugestões desses hiddens pairs dos outros tiles da region
                const auto& suggestionsFromCurrentSolverTile =
                    currentSolverTileWith2Suggestions->getSuggestions();

                const std::vector<TileValueType> suggestionsToBeRemoved(
                    suggestionsFromCurrentSolverTile.cbegin(), suggestionsFromCurrentSolverTile.cend());

                const SolverTileVec solverTilesWithNakedPairs(
                    {currentSolverTileWith2Suggestions, *foundSolverTileWithEqualSuggestions});

                const bool localPerformed =
                    region->removeSuggestionsFromTiles(suggestionsToBeRemoved, /*exceptFromTiles=*/
                                                       solverTilesWithNakedPairs);

                performed |= localPerformed;

                if (localPerformed)
                {
                    m_solver.report(
                        "Naked Pairs:\nA região \"{}\" possui dois tiles com sugestões iguais, no "
                        "Tile {} e {}. Essas sugestões são: {}. Dessa forma, as sugestões dos "
                        "outros tiles da região foram removidas.",
                        *region,
                        *currentSolverTileWith2Suggestions,
                        **foundSolverTileWithEqualSuggestions,
                        joinContainer(currentSolverTileWith2Suggestions->getSuggestions()));

                    break;
                }
            }

            ++it;
        }
    }

    return performed;
}
