#include "SinglesRegion.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"

#include <algorithm>
#include <iterator>

bool SinglesRegion::analyze()
{
    return true;
}

bool SinglesRegion::perform()
{
    bool performed = false;
    for (auto&& region : m_solver.getAllRegions())
    {
        auto& suggestionNum = region->getSuggestionsQuan();
        // procura na region, entre os numero de sugestões para cada valor,
        // se há algum com apenas 1 (single)
        auto found = std::find_if(suggestionNum.cbegin(),
                                  suggestionNum.cend(),
                                  [](const auto& in) -> bool { return in.second == 1; });
        if (found != suggestionNum.cend())
        {
            // a key desse iterator será o valor que tem apenas 1 sugestão
            const TileValueType value = found->first;
            // sempre vai achar
            auto foundTile = std::find_if(region->cbegin(), region->cend(), [&](auto& in) -> bool {
                const auto& suggestions =
                    std::dynamic_pointer_cast<SolverTile>(in)->getSuggestions();
                return suggestions.find(value) != suggestions.cend();
            });
            assert(foundTile != region->cend());
            auto solverTile = std::dynamic_pointer_cast<SolverTile>(*foundTile);
            solverTile->setValue(value);
            performed = true;

            m_solver.report(
                "Singles-Region:\nA região \"{}\" apresenta somente 1 sugestão do "
                "valor {}, no Tile {}. Dessa forma, esse Tile foi definido com esse valor.",
                *region,
                value,
                **foundTile);
        }
    }
    return performed;
}