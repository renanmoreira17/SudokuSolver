#include <catch2/catch_test_macros.hpp>

#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"

static ComponentsConstructor instance;

TEST_CASE("Create Tile from ComponentsConstructor", "[ComponentsConstructor]" ) {
    Grid* mocGrid = (Grid*)1234;
    const auto tile = instance.createTile(mocGrid, 1, 2);
    REQUIRE(tile != nullptr);
    SECTION("Check if Grid address was passed to it") {
        REQUIRE(tile->getGrid() == mocGrid);
    }
    SECTION("Check if row and column was passed to it") {
        const auto coord = tile->getCoordinates();
        REQUIRE(coord.row == 1);
        REQUIRE(coord.col == 2);
    }
    SECTION("Test changing tile") {
        tile->setValue(5);
        REQUIRE(tile->getValue() == 5);
    }
}

TEST_CASE("Create Line from ComponentsConstructor", "[ComponentsConstructor]" ) {
    Grid grid;
    const auto line = instance.createLine(&grid, LineOrientation::HORIZONTAL, 3);
    REQUIRE(line);
    SECTION("Check if index was passed to it") {
        REQUIRE(line->getIndex() == 3);
    }
    SECTION("Check if Grid address was passed to it") {
        REQUIRE(line->getGrid() == &grid);
    }
    SECTION("Check if line orientation was passed to it") {
        REQUIRE(line->getLineOrientation() == LineOrientation::HORIZONTAL);
    }
}

TEST_CASE("Create Subgrid from ComponentsConstructor", "[ComponentsConstructor]" ) {
    Grid grid;
    const auto subgrid = instance.createSubgrid(&grid, 4);
    REQUIRE(subgrid != nullptr);
    SECTION("Check if Grid address was passed to it") {
        REQUIRE(subgrid->getGrid() == &grid);
    }
    SECTION("Check if index was passed to it") {
        REQUIRE(subgrid->getIndex() == 4);
    }
}
