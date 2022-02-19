#include <catch2/catch_test_macros.hpp>

#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"

TEST_CASE("Test tile creation", "[Tile]")
{
    Grid grid;
    SECTION("Check Tile creation without value")
    {
        const Tile tile(&grid, Coordinates{1, 2});
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(tile.getGrid() == &grid);
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE_FALSE(tile.hasValue());
    }
    SECTION("Check Tile creaton with value")
    {
        const Tile tile(&grid, Coordinates{1, 2}, 2);
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(tile.getGrid() == &grid);
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 2);
    }
}

TEST_CASE("Test tile region manipulation", "[Tile]")
{
    Grid grid;
    Tile tile(&grid, Coordinates{1, 2});
    REQUIRE(tile.getHorizontalLine() == nullptr);
    REQUIRE(tile.getVerticalLine() == nullptr);
    REQUIRE(tile.getSubgrid() == nullptr);

    SECTION("Test adding horizontal line")
    {
        Line* hLine = (Line*)0x1234;
        tile.setHorizontalLine(hLine);
        REQUIRE(tile.getHorizontalLine() == hLine);
    }

    SECTION("Test adding vertical line")
    {
        Line* vLine = (Line*)0x1234;
        tile.setVerticalLine(vLine);
        REQUIRE(tile.getVerticalLine() == vLine);
    }

    SECTION("Test adding subgrid")
    {
        Subgrid* subgrid = (Subgrid*)0x2143;
        tile.setSubgrid(subgrid);
        REQUIRE(tile.getSubgrid() == subgrid);
    }

    SECTION("Test getting all regions - tuple")
    {
        Line* hLine = (Line*)0x1234;
        Line* vLine = (Line*)0x4321;
        Subgrid* subgrid = (Subgrid*)0x2143;
        tile.setHorizontalLine(hLine);
        tile.setVerticalLine(vLine);
        tile.setSubgrid(subgrid);
        const auto [fHLine, fVLine, fSubgrid] = tile.getRegionsTuple();
        REQUIRE(hLine == fHLine);
        REQUIRE(vLine == fVLine);
        REQUIRE(subgrid == fSubgrid);
    }

    SECTION("Test getting all regions - array")
    {
        Line hLine(&grid, LineOrientation::HORIZONTAL, 1);
        Line vLine(&grid, LineOrientation::VERTICAL, 2);
        Subgrid subgrid(&grid, 3);
        tile.setHorizontalLine(&hLine);
        tile.setVerticalLine(&vLine);
        tile.setSubgrid(&subgrid);

        const std::vector<Region*> allRegions = {&hLine, &vLine, &subgrid};
        const auto fRegions = tile.getRegions();
        const std::vector<Region*> vfRegions(fRegions.begin(), fRegions.end());
        REQUIRE(allRegions == vfRegions);
    }
}

TEST_CASE("Test tile value manipulation", "[Tile]")
{
    Grid grid;

    SECTION("Test adding value to empty tile")
    {
        Tile tile(&grid, Coordinates{1, 2});
        REQUIRE_FALSE(tile.hasValue());
        REQUIRE(tile.getValue() == 0);
        tile.setValue(2);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 2);
    }

    SECTION("Test adding value to a tile that already has a value")
    {
        Tile tile(&grid, Coordinates{1, 2}, 3);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 3);
        tile.setValue(4);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 4);
    }
}

TEST_CASE("Test Tile move constructor", "[Tile]")
{
    Grid grid;

    const auto& tile = grid(1, 2);
    Tile tile2(tile);
    REQUIRE(tile2.getCoordinates() == tile.getCoordinates());
    REQUIRE(tile2.getValue() == tile.getValue());
    REQUIRE(tile2.getGrid() == nullptr);
    REQUIRE(tile2.getHorizontalLine() == nullptr);
    REQUIRE(tile2.getVerticalLine() == nullptr);
    REQUIRE(tile2.getSubgrid() == nullptr);
}
