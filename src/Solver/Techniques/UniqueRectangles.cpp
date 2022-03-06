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


namespace
{
    class UniqueRectanglesAnalyzer
    {
        
    };
}


bool UniqueRectangles::analyze()
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

bool UniqueRectangles::perform()
{
    for (const auto& vLine : m_solver.getAllSolverVerticalLines())
    {
        const auto& sugQuan = vLine->getSuggestionsQuan();
        const auto candidateSuggestions = sugQuan.getSuggestionsWithQuantityGreaterThan(1);
        if (candidateSuggestions.size() < 2)
        {
            continue;
        }

        const auto combinations = SolverUtils::createCombination(2, candidateSuggestions);
        for (const auto& combination : combinations)
        {
            const auto tiles = vLine->getTilesWithAllSuggestions(combination);
            if (tiles.size() != 2)
            {
                continue;
            }


        }
    }

    std::unordered_set<TileValueType> candidateSuggestionsRaw;
    for (const auto& vLine : m_solver.getAllSolverVerticalLines())
    {
        const auto& sugQuan = vLine->getSuggestionsQuan();
        const auto suggs = sugQuan.getSuggestionsWithQuantityGreaterThan(1);
        candidateSuggestionsRaw.insert(suggs.begin(), suggs.end());
    }
    if (candidateSuggestionsRaw.empty())
    {
        return false;
    }

    const std::vector<TileValueType> candidateSuggestionsRawVector(candidateSuggestionsRaw.begin(),
                                                                   candidateSuggestionsRaw.end());
    const auto combinations = SolverUtils::createCombination(2, candidateSuggestionsRawVector);

    for (const auto& combination : combinations) {}
}
