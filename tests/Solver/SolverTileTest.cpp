#include <catch2/catch_test_macros.hpp>

#include "Solver/SolverRegions.hpp"
#include "Solver/SolverTile.hpp"

TEST_CASE("SolverTile creation", "[SolverTile]")
{
    SECTION("Without value")
    {
        const SolverTile tile(Coordinates{1, 2});
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE_FALSE(tile.hasValue());
    }

    SECTION("With value")
    {
        const SolverTile tile(Coordinates{1, 2}, 7);
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 7);
    }
}

TEST_CASE("SolverTile region pointers", "[SolverTile]")
{
    auto tile = std::make_shared<SolverTile>(Coordinates{1, 2});

    REQUIRE(tile->getSolverHorizontalLine() == nullptr);
    REQUIRE(tile->getSolverVerticalLine() == nullptr);
    REQUIRE(tile->getSolverSubgrid() == nullptr);

    SolverTileVec tiles{tile};

    SolverLine hLine(LineOrientation::HORIZONTAL, 1, tiles);
    SolverLine vLine(LineOrientation::VERTICAL, 2, tiles);
    SolverSubgrid subgrid(3, tiles);

    tile->setHorizontalLine(&hLine);
    tile->setVerticalLine(&vLine);
    tile->setSubgrid(&subgrid);

    REQUIRE(tile->getSolverHorizontalLine() == &hLine);
    REQUIRE(tile->getSolverVerticalLine() == &vLine);
    REQUIRE(tile->getSolverSubgrid() == &subgrid);

    const auto regions = tile->getSolverRegions();
    REQUIRE(regions.size() == 3);
    REQUIRE(regions[0] == &hLine);
    REQUIRE(regions[1] == &vLine);
    REQUIRE(regions[2] == &subgrid);
}

TEST_CASE("SolverTile suggestion manipulation", "[SolverTile]")
{
    SolverTile tile(Coordinates{4, 4});

    REQUIRE(tile.getSuggestions().empty());
    tile.addSuggestion(3);
    tile.addSuggestion(7);
    REQUIRE(tile.getSuggestionsCount() == 2);
    REQUIRE(tile.hasSuggestion(3));
    REQUIRE(tile.hasSuggestion(7));

    REQUIRE(tile.removeSuggestion(3));
    REQUIRE_FALSE(tile.hasSuggestion(3));
    REQUIRE(tile.getSuggestionsCount() == 1);

    tile.addSuggestion(2);
    tile.addSuggestion(5);
    REQUIRE(tile.removeSuggestions({2, 5}));
    REQUIRE(tile.getSuggestionsCount() == 1);

    tile.addSuggestion(9);
    tile.removeAllSuggestionsExceptFrom({9});
    REQUIRE(tile.getSuggestionsCount() == 1);
    REQUIRE(tile.hasSuggestion(9));
}
