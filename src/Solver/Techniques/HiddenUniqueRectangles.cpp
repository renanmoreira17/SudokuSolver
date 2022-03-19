#include "HiddenUniqueRectangles.hpp"

#include "Solver/Solver.hpp"
#include "Solver/SolverTile.hpp"
#include "Solver/SolverUtils.hpp"
#include "Tools/UniqueRectanglesCreator.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <fmt/format.h>

namespace
{
class HiddenUniqueRectanglesAnalyzer
{
  public:
    HiddenUniqueRectanglesAnalyzer(SolverTileVec tiles,
                                   std::vector<TileValueType> biValueSuggestions,
                                   const Solver& solver)
        : m_solver(solver)
        , m_biValueSuggestions(std::move(biValueSuggestions))
        , m_tiles(std::move(tiles))
    {
        assert(m_tiles.size() == 4);
        assert(m_biValueSuggestions.size() == 2);
    }

    bool analyzeAndPerform() const
    {
        if (analyzeType1())
        {
            return true;
        }
        if (analyzeType2())
        {
            return true;
        }
        return false;
    }

  private:
    bool analyzeType1() const
    {
        // for type 1, there must be one tile with bi value suggestions
        const auto quanBiValue =
            std::count_if(m_tiles.begin(), m_tiles.end(), [&](const SolverTilePtr& tile) {
                return tile->getSuggestionsCount() == 2;
            });
        if (quanBiValue != 1)
        {
            return false;
        }

        // furthermore, the tile opposed to the one with bi value suggestions must form a strong link, from
        // one of those bi value suggestions to the suggestions of the remaining rectangle tiles
        const auto tileWithBiValue =
            std::find_if(m_tiles.begin(), m_tiles.end(), [&](const SolverTilePtr& tile) {
                return tile->getSuggestionsCount() == 2;
            });
        const auto tileWithBiValueIndex = std::distance(m_tiles.begin(), tileWithBiValue);
        const auto opposedTileIndex =
            tileWithBiValueIndex > 1 ? tileWithBiValueIndex - 2 : tileWithBiValueIndex + 2;
        const auto opposedTile = m_tiles.at(opposedTileIndex);
        assert(SolverUtils::getTilesCommonRegion(**tileWithBiValue, *opposedTile) == nullptr);

        SolverTileVec otherTiles;
        std::copy_if(
            m_tiles.begin(), m_tiles.end(), std::back_inserter(otherTiles), [&](const SolverTilePtr& tile) {
                return tile != *tileWithBiValue && tile != opposedTile;
            });
        assert(otherTiles.size() == 2);

        // now check the strong link for each of the original bi-value suggestions
        for (const auto& biValueSuggestion : m_biValueSuggestions)
        {
            bool isStrongLink = true;
            for (const auto& otherTile : otherTiles)
            {
                const auto commonLine = SolverUtils::getCommonSolverLine(opposedTile, otherTile);
                assert(commonLine);
                const auto quan = commonLine->getSuggestionsQuanFor(biValueSuggestion);
                const bool areTilesStrongLinked = quan == 2;
                if (!areTilesStrongLinked)
                {
                    isStrongLink = false;
                    break;
                }
            }
            if (isStrongLink)
            {
                const auto otherSuggestion = biValueSuggestion == m_biValueSuggestions.front()
                                                 ? m_biValueSuggestions.back()
                                                 : m_biValueSuggestions.front();
                const auto erased = opposedTile->removeSuggestion(otherSuggestion);
                assert(erased);
                m_solver.report(
                    "Hidden Unique Rectangles - Type 1:\nOs tiles {} formam um Hidden Unique Rectangle, com "
                    "as sugestões {}. O tile {} é o único com sugestões bi-value e, além disso, o tile "
                    "oposto a ele, {}, possui strong links da sugestão {} com os tiles {} (que fazem parte "
                    "do Unique Rectangle). Dessa forma, para se evitar um deadly pattern, a outra sugestão "
                    "que não forma strong links deve ser removida desse tile oposto. Assim, a sugestão {} "
                    "foi removida do tile {}.",
                    fmt::join(m_tiles, ", "),
                    fmt::join(m_biValueSuggestions, ", "),
                    *tileWithBiValue,
                    opposedTile,
                    biValueSuggestion,
                    fmt::join(otherTiles, ", "),
                    otherSuggestion,
                    *opposedTile);
                return true;
            }
        }

        return false;
    }

    bool analyzeType2() const
    {
        SolverTileVec floor, roof;
        for (const auto& tile : m_tiles)
        {
            if (tile->getSuggestionsCount() == 2)
            {
                floor.push_back(tile);
            }
            else
            {
                roof.push_back(tile);
            }
        }
        // in order to be type 2, there must be two floor tiles and two roof tiles.
        if (floor.size() != 2 || roof.size() != 2)
        {
            return false;
        }

        // and also, the floor must be a naked pair itself
        if (!SolverUtils::areTilesInAnySameLine(*floor.front(), *floor.back()))
        {
            return false;
        }

        // test for each floor
        for (const auto& floorTile : floor)
        {
            // and for each bi-value suggestion option
            for (const auto& suggestion : m_biValueSuggestions)
            {
                // get which of the roof tiles is in the same line as the current floor
                const auto roofTile = std::find_if(roof.begin(), roof.end(), [&](const SolverTilePtr& tile) {
                    return SolverUtils::getCommonSolverLine(floorTile, tile) != nullptr;
                });
                assert(roofTile != roof.end());
                const auto* commonLine = SolverUtils::getCommonSolverLine(floorTile, *roofTile);
                assert(commonLine);
                // check if both tiles are strong linked through the current bi-value suggestion
                const auto quan = commonLine->getSuggestionsQuanFor(suggestion);
                if (quan == 2)
                {
                    const auto otherSuggestion = suggestion == m_biValueSuggestions.front()
                                                     ? m_biValueSuggestions.back()
                                                     : m_biValueSuggestions.front();
                    const auto otherRoof = roofTile == roof.begin() ? roof.back() : roof.front();
                    // if so, remove the suggestion from the floor tile
                    const auto erased = otherRoof->removeSuggestion(otherSuggestion);
                    assert(erased);
                    m_solver.report(
                        "Hidden Unique Rectangles - Type 2:\nOs tiles {} formam um Hidden Unique Rectangle, "
                        "com as sugestões {}. Os tiles {} formam o floor, com somente as sugestões bi-value, "
                        "e além disso são um Naked Pair. Desse floor, se analisarmos o tile {}, veremos que "
                        "ele faz um strong link da sugestão {} com o roof tile {}, através da linha {}. "
                        "Assim, para se evitar um deadly pattern, devemos eliminar a outra sugestão de "
                        "bi-value do outro roof tile que não forma o strong link. Dessa forma, a sugestão {} "
                        "foi removida do tile {}.",
                        fmt::join(m_tiles, ", "),
                        fmt::join(m_biValueSuggestions, ", "),
                        fmt::join(floor, ", "),
                        floorTile,
                        suggestion,
                        *roofTile,
                        *commonLine,
                        otherSuggestion,
                        otherRoof);
                    return true;
                }
            }
        }

        return false;
    }

    const Solver& m_solver;
    const std::vector<TileValueType> m_biValueSuggestions;
    const SolverTileVec m_tiles;
};
} // namespace

bool HiddenUniqueRectangles::analyze()
{
    std::unordered_map<TileValueType, int> suggestionsPerValue;
    for (const auto& vLine : m_solver.getAllSolverVerticalLines())
    {
        const auto& suggestionsQuan = vLine->getSuggestionsQuan();
        const auto& candidateSuggestions = suggestionsQuan.getSuggestionsWithQuantityGreaterThan(1);
        if (candidateSuggestions.size() >= 2)
        {
            return true;
        }
    }
    return false;
}

bool HiddenUniqueRectangles::perform()
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
            // we must also check if there are at least 2 tiles with 2 suggestions to constitute the
            // floor tiles
            const UniqueRectanglesCreator URCreator(combination, vLine);
            const auto possibleRectangles = URCreator.buildPossibleRectangles();

            for (const auto& possibleRectangle : possibleRectangles)
            {
                unsigned short quanWith2Suggestions = 0;
                for (const auto& tile : possibleRectangle)
                {
                    if (tile->getSuggestionsCount() == 2)
                    {
                        ++quanWith2Suggestions;
                    }
                }
                if (quanWith2Suggestions < 1)
                {
                    continue;
                }

                const HiddenUniqueRectanglesAnalyzer analyzer(possibleRectangle, combination, m_solver);
                if (analyzer.analyzeAndPerform())
                {
                    return true;
                }
            }
        }
    }
    return false;
}
