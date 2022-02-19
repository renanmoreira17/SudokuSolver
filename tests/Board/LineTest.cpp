#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Tile.hpp"

TEST_CASE("Test Line creation", "[Line]")
{
    Grid gridMoc;
    SECTION("Horizontal Line")
    {
        Line line(&gridMoc, LineOrientation::HORIZONTAL, 1);
        REQUIRE(line.getGrid() == &gridMoc);
        REQUIRE(line.getIndex() == 1);
        REQUIRE(line.getType() == Region::RegionType::LINE);
        REQUIRE(line.getLineOrientation() == LineOrientation::HORIZONTAL);
        REQUIRE(line.getRegionSpecificType() == RegionSpecificType::HORIZONTAL_LINE);
    }

    SECTION("Vertical Line")
    {
        Line line(&gridMoc, LineOrientation::VERTICAL, 2);
        REQUIRE(line.getGrid() == &gridMoc);
        REQUIRE(line.getIndex() == 2);
        REQUIRE(line.getType() == Region::RegionType::LINE);
        REQUIRE(line.getLineOrientation() == LineOrientation::VERTICAL);
        REQUIRE(line.getRegionSpecificType() == RegionSpecificType::VERTICAL_LINE);
    }
}

TEST_CASE("Test Line Tiles creation from Grid", "[Line]")
{
    auto refRowCol = GENERATE(range(0, 8));
    Grid gridMoc;

    SECTION("Horizontal")
    {
        Line hLine(&gridMoc, LineOrientation::HORIZONTAL, refRowCol);
        std::vector<std::shared_ptr<Tile>> refTiles;
        for (int col = 0; col < 9; ++col) { refTiles.push_back(gridMoc(refRowCol, col)); }
        const auto& elementList = hLine.getTiles();
        std::vector<std::shared_ptr<Tile>> createdTiles(elementList.begin(), elementList.end());
        REQUIRE(refTiles == createdTiles);
    }

    SECTION("Vertical")
    {
        Line vLine(&gridMoc, LineOrientation::VERTICAL, refRowCol);

        std::vector<std::shared_ptr<Tile>> refTiles;
        for (int row = 0; row < 9; ++row) { refTiles.emplace_back(gridMoc(row, refRowCol)); }
        const auto& elementList = vLine.getTiles();
        std::vector<std::shared_ptr<Tile>> createdTiles(elementList.begin(), elementList.end());
        REQUIRE(refTiles == createdTiles);
    }
}

TEST_CASE("Test if Line's Tiles have their region correctly setted", "[Line]")
{
    Grid gridMoc;

    auto refRowCol = GENERATE(range(0, 8));

    SECTION("Horizontal")
    {
        Line line(&gridMoc, LineOrientation::HORIZONTAL, refRowCol);

        for (const auto& tile : line.getTiles()) { REQUIRE(tile->getHorizontalLine() == &line); }
    }

    SECTION("Vertical")
    {
        Line line(&gridMoc, LineOrientation::VERTICAL, refRowCol);

        for (const auto& tile : line.getTiles()) { REQUIRE(tile->getVerticalLine() == &line); }
    }
}

TEST_CASE("Test Line move constructors/assign operators", "[Line]")
{
    Grid gridMoc;
    auto refRowCol = GENERATE(range(0, 8));

    SECTION("Contructor - Horizontal")
    {
        Line line(&gridMoc, LineOrientation::HORIZONTAL, refRowCol);
        Line lineMoved(std::move(line));
        REQUIRE(lineMoved.getGrid() == &gridMoc);
        REQUIRE(lineMoved.getIndex() == refRowCol);
        REQUIRE(lineMoved.getType() == Region::RegionType::LINE);
        REQUIRE(lineMoved.getLineOrientation() == LineOrientation::HORIZONTAL);
        REQUIRE(lineMoved.getRegionSpecificType() == RegionSpecificType::HORIZONTAL_LINE);
        for (const auto& tile : lineMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getHorizontalLine() == &lineMoved);
        }

        REQUIRE_FALSE(line.getElementList());
    }

    SECTION("Contructor - Vertical")
    {
        Line line(&gridMoc, LineOrientation::VERTICAL, refRowCol);
        Line lineMoved(std::move(line));
        REQUIRE(lineMoved.getGrid() == &gridMoc);
        REQUIRE(lineMoved.getIndex() == refRowCol);
        REQUIRE(lineMoved.getType() == Region::RegionType::LINE);
        REQUIRE(lineMoved.getLineOrientation() == LineOrientation::VERTICAL);
        REQUIRE(lineMoved.getRegionSpecificType() == RegionSpecificType::VERTICAL_LINE);
        for (const auto& tile : lineMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getVerticalLine() == &lineMoved);
        }

        REQUIRE_FALSE(line.getElementList());
    }

    SECTION("Operator - Horizontal")
    {
        Line line(&gridMoc, LineOrientation::HORIZONTAL, refRowCol);
        Line lineMoved = std::move(line);
        REQUIRE(lineMoved.getGrid() == &gridMoc);
        REQUIRE(lineMoved.getIndex() == refRowCol);
        REQUIRE(lineMoved.getType() == Region::RegionType::LINE);
        REQUIRE(lineMoved.getLineOrientation() == LineOrientation::HORIZONTAL);
        REQUIRE(lineMoved.getRegionSpecificType() == RegionSpecificType::HORIZONTAL_LINE);
        for (const auto& tile : lineMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getHorizontalLine() == &lineMoved);
        }

        REQUIRE_FALSE(line.getElementList());
    }

    SECTION("Operator - Vertical")
    {
        Line line(&gridMoc, LineOrientation::VERTICAL, refRowCol);

        Grid auxGrid;
        Line lineMoved(&auxGrid, LineOrientation::VERTICAL, refRowCol);
        lineMoved = std::move(line);

        REQUIRE(lineMoved.getGrid() == &gridMoc);
        REQUIRE(lineMoved.getIndex() == refRowCol);
        REQUIRE(lineMoved.getType() == Region::RegionType::LINE);
        REQUIRE(lineMoved.getLineOrientation() == LineOrientation::VERTICAL);
        REQUIRE(lineMoved.getRegionSpecificType() == RegionSpecificType::VERTICAL_LINE);
        for (const auto& tile : lineMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getVerticalLine() == &lineMoved);
        }

        REQUIRE_FALSE(line.getElementList());
    }
}
