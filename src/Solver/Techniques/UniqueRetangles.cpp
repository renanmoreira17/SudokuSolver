#include "UniqueRetangles.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverUtils.hpp"
#include "Tools/Chain.hpp"
#include "Tools/Traversal.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <optional>
#include <unordered_set>

#include "Util/UtilFunctions.hpp"
#include <fmt/format.h>
#include <iostream>


bool UniqueRetangles::analyze()
{
    std::unordered_map<TileValueType, int> suggestionsPerValue;
    for (const auto& region : m_solver.getAllRegions())
    {
        const auto& suggestionsQuan = region->getSuggestionsQuan();
        const auto& suggestionsWith2Appearances = suggestionsQuan.getSuggestionsWithQuantityEqualTo(2);
        for (const auto& suggestion : suggestionsWith2Appearances)
        {
            ++suggestionsPerValue[suggestion];
            if (suggestionsPerValue[suggestion] >= 3)
            {
                return true;
            }
        }
    }
    return false;
}

bool UniqueRetangles::perform()
{
}

