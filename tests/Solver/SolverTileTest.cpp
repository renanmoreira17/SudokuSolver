#include <catch2/catch_test_macros.hpp>

#include "Board/Grid.hpp"
#include "Solver/SolverRegions.hpp"
#include "Solver/SolverTile.hpp"

TEST_CASE("Test SolverTile creation", "[SolverTile]")
{
    Grid grid;
    SECTION("Check SolverTile creation without value")
    {
        const SolverTile tile(&grid, Coordinates{1, 2});
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(tile.getGrid() == &grid);
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE_FALSE(tile.hasValue());
    }
    SECTION("Check SolverTile creaton with value")
    {
        const SolverTile tile(&grid, Coordinates{1, 2}, 2);
        const auto& coordinates = tile.getCoordinates();
        REQUIRE(tile.getGrid() == &grid);
        REQUIRE(coordinates.row == 1);
        REQUIRE(coordinates.col == 2);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 2);
    }
}

TEST_CASE("Test SolverTile region manipulation", "[SolverTile]")
{
    Grid grid;
    SolverTile tile(&grid, Coordinates{1, 2});
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

TEST_CASE("Test SolverTile SolverRegion manipulation", "[SolverTile]")
{
    Grid grid;
    SolverTile tile(&grid, Coordinates{1, 2});
    REQUIRE(tile.getHorizontalLine() == nullptr);
    REQUIRE(tile.getVerticalLine() == nullptr);
    REQUIRE(tile.getSubgrid() == nullptr);

    SECTION("Test adding horizontal line")
    {
        SolverLine* hLine = (SolverLine*)0x1234;
        tile.setHorizontalLine(hLine);
        REQUIRE(tile.getHorizontalLine() == hLine);
        REQUIRE(tile.getSolverHorizontalLine() == hLine);
    }

    SECTION("Test adding vertical line")
    {
        SolverLine* vLine = (SolverLine*)0x1234;
        tile.setVerticalLine(vLine);
        REQUIRE(tile.getVerticalLine() == vLine);
        REQUIRE(tile.getSolverVerticalLine() == vLine);
    }

    SECTION("Test adding subgrid")
    {
        SolverSubgrid* subgrid = (SolverSubgrid*)0x2143;
        tile.setSubgrid(subgrid);
        REQUIRE(tile.getSubgrid() == subgrid);
        REQUIRE(tile.getSolverSubgrid() == subgrid);
    }

    SECTION("Test getting all SolverRegions")
    {
        SolverLine hLine(&grid, LineOrientation::HORIZONTAL, 1);
        SolverLine vLine(&grid, LineOrientation::VERTICAL, 2);
        SolverSubgrid subgrid(&grid, 3);
        tile.setHorizontalLine(&hLine);
        tile.setVerticalLine(&vLine);
        tile.setSubgrid(&subgrid);

        const std::vector<Region*> allRegions = {&hLine, &vLine, &subgrid};
        const auto fRegions = tile.getRegions();
        const std::vector<Region*> vfRegions(fRegions.begin(), fRegions.end());
        REQUIRE(allRegions == vfRegions);
    }
}

TEST_CASE("Test SolverTile value manipulation", "[SolverTile]")
{
    Grid grid;

    SECTION("Test adding value to empty tile")
    {
        SolverTile tile(&grid, Coordinates{1, 2});
        REQUIRE_FALSE(tile.hasValue());
        REQUIRE(tile.getValue() == 0);
        tile.setValue(2);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 2);
    }

    SECTION("Test adding value to a tile that already has a value")
    {
        SolverTile tile(&grid, Coordinates{1, 2}, 3);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 3);
        tile.setValue(4);
        REQUIRE(tile.hasValue());
        REQUIRE(tile.getValue() == 4);
    }
}

TEST_CASE("Test SolverTile move constructor", "[SolverTile]")
{
    Grid grid;

    const auto& tile = grid(1, 2);
    SolverTile tile2(*tile);
    REQUIRE(tile2.getCoordinates().col == tile->getCoordinates().col);
    REQUIRE(tile2.getCoordinates().row == tile->getCoordinates().row);
    REQUIRE(tile2.getValue() == tile->getValue());
    REQUIRE(tile2.getGrid() == nullptr);
    REQUIRE(tile2.getHorizontalLine() == nullptr);
    REQUIRE(tile2.getVerticalLine() == nullptr);
    REQUIRE(tile2.getSubgrid() == nullptr);
}
