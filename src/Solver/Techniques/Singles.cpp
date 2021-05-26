#include "Singles.hpp"
#include "Solver/Solver.hpp"
#include <algorithm>
#include <iterator>

bool Singles::analyze()
{
    return true;
}

bool Singles::perform()
{
    bool performed = false;
    for (auto &&region : m_solver->getAllRegions())
    {
        auto& suggestionNum = region->getSuggestionsQuan();
        // procura na region, entre os numero de sugestões para cada valor,
        // se há algum com apenas 1 (single)
        auto found = std::find_if(suggestionNum.cbegin(), suggestionNum.cend(), 
            [](const auto& in) -> bool { return in.second == 1; });
        if (found != suggestionNum.cend())
        {
            auto value = std::distance(suggestionNum.cbegin(), found)+1;
            // sempre vai achar
            auto foundTile = std::find_if(region->cbegin(), region->cend(),
                [&](auto& in)->bool
                {
                    return in.second.find(value) != in.second.cend();
                }
            );
            // auto& originalRegion = region->getOriginalRegion();
            m_solver->placeValueInTile(foundTile->first, value);
            performed = true;
        }
    }
    return performed;
}