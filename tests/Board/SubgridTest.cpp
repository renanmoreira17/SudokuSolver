#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

#include "Board/Grid.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"
#include "Util/UtilFunctions.hpp"

TEST_CASE("Test Subgrid creation", "[Subgrid]")
{
    Grid gridMoc;
    auto refIndex = GENERATE(range(0, 8));
    Subgrid subgrid(&gridMoc, refIndex);
    REQUIRE(subgrid.getGrid() == &gridMoc);
    REQUIRE(subgrid.getIndex() == refIndex);
    REQUIRE(subgrid.getType() == Region::RegionType::SUBGRID);
    REQUIRE(subgrid.getRegionSpecificType() == RegionSpecificType::SUBGRID);
}

TEST_CASE("Test Subgrid's Tiles creation from Grid", "[Subgrid]")
{
    auto refIndex = GENERATE(range(0, 8));
    Grid gridMoc;

    Subgrid subgrid(&gridMoc, refIndex);

    std::vector<std::shared_ptr<Tile>> refTiles;
    for (int index = 0; index < 9; ++index)
    {
        const auto coord = transformSubgridIndexedToAbsoluteCoordinates(index, refIndex);
        refTiles.push_back(gridMoc(coord.row, coord.col));
    }
    const auto& elementList = subgrid.getTiles();
    std::vector<std::shared_ptr<Tile>> createdTiles(elementList.begin(), elementList.end());
    REQUIRE(refTiles == createdTiles);
}

TEST_CASE("Test if Subgrid's Tiles have their region correctly setted", "[Subgrid]")
{
    Grid gridMoc;

    auto refIndex = GENERATE(range(0, 8));

    Subgrid subgrid(&gridMoc, refIndex);
    for (const auto& tile : subgrid.getTiles()) { REQUIRE(tile->getSubgrid() == &subgrid); }
}

TEST_CASE("Test Subgrid move constructors/assign operators", "[Subgrid]")
{
    Grid gridMoc;
    auto refIndex = GENERATE(range(0, 8));

    SECTION("Contructor")
    {
        Subgrid subgrid(&gridMoc, refIndex);
        Subgrid subgridMoved(std::move(subgrid));
        REQUIRE(subgridMoved.getGrid() == &gridMoc);
        REQUIRE(subgridMoved.getIndex() == refIndex);
        REQUIRE(subgridMoved.getType() == Region::RegionType::SUBGRID);
        REQUIRE(subgridMoved.getRegionSpecificType() == RegionSpecificType::SUBGRID);
        for (const auto& tile : subgridMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getSubgrid() == &subgridMoved);
        }

        REQUIRE_FALSE(subgrid.getElementList());
    }

    SECTION("Operator")
    {
        Subgrid subgrid(&gridMoc, refIndex);

        Grid auxGrid;
        Subgrid subgridMoved(&auxGrid, refIndex);
        subgridMoved = std::move(subgrid);

        REQUIRE(subgridMoved.getGrid() == &gridMoc);
        REQUIRE(subgridMoved.getIndex() == refIndex);
        REQUIRE(subgridMoved.getType() == Region::RegionType::SUBGRID);
        REQUIRE(subgridMoved.getRegionSpecificType() == RegionSpecificType::SUBGRID);
        for (const auto& tile : subgridMoved.getTiles())
        {
            REQUIRE(tile == gridMoc(tile->getCoordinates().row, tile->getCoordinates().col));
            REQUIRE(tile->getSubgrid() == &subgridMoved);
        }

        REQUIRE_FALSE(subgrid.getElementList());
    }
}
