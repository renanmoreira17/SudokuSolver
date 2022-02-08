#include "PointingPair.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverUtils.hpp"

#include <algorithm>
#include <cassert>

bool PointingPair::analyze()
{
    for (const auto& subgrid : m_solver.getAllSolverSubgrids())
    {
        const auto& subgridSuggestionsQuan = subgrid->getSuggestionsQuan();
        const bool isThereAtLeastOneSuggestionWith2Appearances =
            std::any_of(subgridSuggestionsQuan.begin(),
                        subgridSuggestionsQuan.end(),
                        [](const auto& quanPair) { return quanPair.second == 2; });
        if (isThereAtLeastOneSuggestionWith2Appearances)
        {
            return true;
        }
    }
    return false;
}

bool PointingPair::perform()
{
    bool performed = false;
    for (const auto& subgrid : m_solver.getAllSolverSubgrids())
    {
        const auto& subgridSuggestionsQuan = subgrid->getSuggestionsQuan();

        std::vector<TileValueType> suggestionsWith2Appearances;
        for (const auto& suggestionQuanPair : subgridSuggestionsQuan)
        {
            if (suggestionQuanPair.second == 2)
            {
                suggestionsWith2Appearances.push_back(suggestionQuanPair.first);
            }
        }

        for (const auto& suggestion : suggestionsWith2Appearances)
        {
            const auto& tilesWithSuggestion = subgrid->getTilesWithSuggestion(suggestion);
            assert(tilesWithSuggestion.size() == 2);
            const auto& tile1 = tilesWithSuggestion.front();
            const auto& tile2 = tilesWithSuggestion.back();

            const bool areTilesInTheSameVerticalLine =
                SolverUtils::areTilesInTheSameLine(*tile1, *tile2, LineOrientation::VERTICAL);
            const bool areTilesInTheSameHorizontalLine =
                SolverUtils::areTilesInTheSameLine(*tile1, *tile2, LineOrientation::HORIZONTAL);

            if (areTilesInTheSameVerticalLine || areTilesInTheSameHorizontalLine)
            {
                SolverLine* targetLine = areTilesInTheSameVerticalLine
                                             ? tile1->getSolverVerticalLine()
                                             : tile1->getSolverHorizontalLine();
                const auto& targetLineSuggestionsQuan = targetLine->getSuggestionsQuan();
                // if the number of appearances of suggestion in the line being targeted is equal to
                // 2, then there are no other suggestions to remove and therefore this is not a
                // pointing pair
                if (targetLineSuggestionsQuan.getSuggestionsQuantityFor(suggestion) <= 2)
                {
                    continue;
                }
                targetLine->removeSingleSuggestionFromTiles(suggestion, tilesWithSuggestion);
                performed = true;

                m_solver.report(
                    "Pointing Pair:\nO quadrado \"{}\" possui apenas duas sugestões de valor {} em "
                    "sua região. Ademais, essas duas sugestões estão nos Tiles {} e {}, que "
                    "pertencem à mesma linha \"{}\". Dessa forma, essa sugestão deve ser o valor "
                    "de um desses dois Tiles, e a sugestão pode ser removida de todos os outros "
                    "Tiles da linha \"{}\".",
                    *subgrid,
                    suggestion,
                    *tile1,
                    *tile2,
                    *targetLine,
                    *targetLine);
            }
        }
    }

    return performed;
}
