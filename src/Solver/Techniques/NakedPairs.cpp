#include "NakedPairs.hpp"
#include "Solver/Solver.hpp"
#include <type_traits>
#include <list>

bool NakedPairs::analyze()
{
    for (auto &&region : m_solver->getAllRegions())
    {
        short suggestionQuan = 0;
        for (auto &&suggestion : *region)
        {
            if (suggestion.second.size() == 2)
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
    for (auto &&region : m_solver->getAllRegions())
    {
        if (region->isCompleted())
            continue;

        using VecType = std::decay<decltype(region->getElementList()->getHead()->getElement())>::type;
        std::list<VecType*> filteredSuggestions;

        // filtra todos tiles que tem a quantidade de sugestões = 2
        for (auto &&suggestion : *region)
        {
            if (suggestion.second.size() == 2)
                filteredSuggestions.push_back(&suggestion);
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
            auto found = std::find_if(filteredSuggestions.begin(), filteredSuggestions.end(),
                [&](decltype(current) s) -> bool
                {
                    return s != current && s->second == current->second;
                }
            );
            if (found != filteredSuggestions.end())
            {
                // e remove as sugestões desses hiddens pairs dos outros tiles da region
                for (auto &&suggestion : *region)
                {
                    if (&suggestion == current || &suggestion == *found || suggestion.second.empty())
                        continue;
                    for (auto &&currentSugg : current->second)
                    {
                        bool erased = suggestion.second.erase(currentSugg);
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
