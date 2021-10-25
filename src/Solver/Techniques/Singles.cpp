#include "Singles.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"

#include <algorithm>
#include <iterator>

bool Singles::analyze()
{
    return true;
}

bool Singles::perform()
{
    bool performed = false;
    for (auto&& region : m_solver->getAllRegions())
    {
        // iterate over SolverRegion

        auto& suggestionNum = region->getSuggestionsQuan();
        // procura na region, entre os numero de sugestões para cada valor,
        // se há algum com apenas 1 (single)
        auto found = std::find_if(suggestionNum.cbegin(),
                                  suggestionNum.cend(),
                                  [](const auto& in) -> bool { return in.second == 1; });
        if (found != suggestionNum.cend())
        {
            auto value = std::distance(suggestionNum.cbegin(), found) + 1;
            // sempre vai achar
            auto foundTile = std::find_if(region->cbegin(), region->cend(), [&](auto& in) -> bool {
                const auto& suggestions =
                    std::dynamic_pointer_cast<SolverTile>(in)->getSuggestions();
                return suggestions.find(value) != suggestions.cend();
            });
            auto solverTile = std::dynamic_pointer_cast<SolverTile>(*foundTile);
            solverTile->setValue(value);
            performed = true;
        }
    }
    return performed;
}