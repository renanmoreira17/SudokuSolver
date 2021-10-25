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
    for (auto&& region : m_solver->getAllRegions())
    {
        if (region->isCompleted())
            continue;

        std::list<std::shared_ptr<SolverTile>*> filteredSuggestions;

        // filtra todos tiles que tem a quantidade de sugestões = 2
        for (auto&& tile : *region)
        {
            auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
            if (solverTile->getSuggestions().size() == 2)
            {
                filteredSuggestions.push_back(&solverTile);
            }
        }

        if (filteredSuggestions.size() <= 2)
            continue;

        auto it = filteredSuggestions.begin();
        // so pode ser feito uma vez por region, por mais que haja outros hidden pairs na region.
        // isso porque a remoçao de sugestoes de uma region pode interfereir a filteredSuggestions
        // se houver remoçao de uma das sugestoes dos possiveis hidden pairs
        while (it != filteredSuggestions.end())
        {
            auto& current = *it;
            auto found = std::find_if(filteredSuggestions.begin(),
                                      filteredSuggestions.end(),
                                      [&](decltype(current) s) -> bool {
                                          return s != current && (*s)->getSuggestions() ==
                                                                     (*current)->getSuggestions();
                                      });
            if (found != filteredSuggestions.end())
            {
                // e remove as sugestões desses hiddens pairs dos outros tiles da region
                for (auto&& tile : *region)
                {
                    auto solverTile = std::dynamic_pointer_cast<SolverTile>(tile);
                    if (solverTile == *current || solverTile == **found ||
                        solverTile->getSuggestions().empty())
                        continue;
                    for (auto&& currentSugg : (*current)->getSuggestions())
                    {
                        bool erased = solverTile->getSuggestions().erase(currentSugg);
                        performed |= erased;
                    }
                }
                // remove tanto o elemento encontrado
                filteredSuggestions.erase(found);
                // quanto o atual
                it = filteredSuggestions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return performed;
}
