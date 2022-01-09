#include "NakedPairs.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"

#include <list>
#include <memory>
#include <type_traits>
#include <utility>

bool NakedPairs::analyze()
{
    for (auto&& region : m_solver->getAllRegions())
    {
        short suggestionQuan = 0;
        for (auto&& tile : *region)
        {
            // cast tile to std::shared_ptr<SolverTile>, check if the number of suggestions is
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
    for (const auto& region : m_solver->getAllRegions())
    {
        if (region->isCompleted())
            continue;

        std::list<const std::shared_ptr<SolverTile>> filteredSuggestions;

        // filtra todos tiles que tem a quantidade de sugestões == 2
        for (const auto& tile : *region)
        {
            const auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
            if (solverTile->getSuggestions().size() == 2)
            {
                filteredSuggestions.push_back(solverTile);
            }
        }

        if (filteredSuggestions.size() < 2)
            continue;

        auto it = filteredSuggestions.begin();
        // so pode ser feito uma vez por region, por mais que haja outros hidden pairs na region.
        // isso porque a remoçao de sugestoes de uma region pode interfereir a filteredSuggestions
        // se houver remoçao de uma das sugestoes dos possiveis hidden pairs
        while (it != filteredSuggestions.end())
        {
            const auto& current = *it;
            // procura, no restante da lista, se há algum tile que possui a mesma sugestão
            const auto found = std::find_if(
                filteredSuggestions.begin(),
                filteredSuggestions.end(),
                [&](decltype(current) s) -> bool {
                    return s != current && s->getSuggestions() == current->getSuggestions();
                });

            if (found != filteredSuggestions.cend())
            {
                // e remove as sugestões desses hiddens pairs dos outros tiles da region
                for (const std::shared_ptr<Tile>& tile : *region)
                {
                    const auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
                    if (solverTile == current || solverTile == *found ||
                        solverTile->getSuggestions().empty())
                        continue;
                    for (const TileValueType& currentSuggestion : current->getSuggestions())
                    {
                        const bool erased = solverTile->getSuggestions().erase(currentSuggestion);
                        performed |= erased;
                    }
                }
                if (performed)
                {
                    m_solver->report(
                        "Naked Pairs:\nA região \"{}\" possui dois tiles com sugestões iguais, no "
                        "Tile {} e {}. Essas sugestões são: {}. Dessa forma, as sugestões dos "
                        "outros tiles da região foram removidas.",
                        *region,
                        *current,
                        **found,
                        joinContainer(current->getSuggestions()));

                    break;
                }
            }

            ++it;
        }
    }

    return performed;
}
