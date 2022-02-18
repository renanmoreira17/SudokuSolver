#include <catch2/catch_test_macros.hpp>

#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Region.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"

TEST_CASE("Test Region creation", "[Region]")
{
    Grid gridMoc;

    Line line(&gridMoc, LineOrientation::HORIZONTAL, 1);
    Region* region = &line;
    SECTION("Check grid") { REQUIRE(region->getGrid() == &gridMoc); }
    SECTION("Check index") { REQUIRE(region->getIndex() == 1); }
    SECTION("Check type") { REQUIRE(region->getType() == Region::RegionType::LINE); }
    SECTION("Check specific type")
    {
        REQUIRE(region->getRegionSpecificType() == RegionSpecificType::HORIZONTAL_LINE);
    }
}

TEST_CASE("Test Region Tiles-related APIs", "[Region]")
{
    Grid gridMoc;

    auto firstMocTile = std::make_shared<Tile>(&gridMoc, Coordinates{1, 2});
    auto middleMocTile = std::make_shared<Tile>(&gridMoc, Coordinates{1, 2});
    auto lastMocTile = std::make_shared<Tile>(&gridMoc, Coordinates{1, 2});
    auto* first = new ElementWrapper<std::shared_ptr<Tile>>(&firstMocTile);
    auto* middle = new ElementWrapper<std::shared_ptr<Tile>>(&middleMocTile, first);
    auto* last = new ElementWrapper<std::shared_ptr<Tile>>(&lastMocTile, middle);

    auto eList = std::make_shared<ElementList<std::shared_ptr<Tile>>>(first);

    Line line(&gridMoc, LineOrientation::HORIZONTAL, 1);
    Region* region = &line;
    region->setElementList(eList);

    SECTION("Test Region iterator")
    {
        const std::vector<ElementWrapper<std::shared_ptr<Tile>>*> golden = {first, middle, last};
        auto goldenIt = golden.begin();
        for (auto it = region->begin(); it != region->end(); ++it)
        {
            REQUIRE(*it == (*goldenIt)->getElement());
            REQUIRE(&(*it) == &(*goldenIt)->getElement());
            ++goldenIt;
        }
    }

    SECTION("Test Region iterator")
    {
        const std::vector<ElementWrapper<std::shared_ptr<Tile>>*> golden = {first, middle, last};
        auto goldenIt = golden.begin();
        for (auto it = region->begin(); it != region->end(); ++it)
        {
            REQUIRE(*it == (*goldenIt)->getElement());
            REQUIRE(&(*it) == &(*goldenIt)->getElement());
            ++goldenIt;
        }
    }

    SECTION("Test hasValue")
    {
        REQUIRE_FALSE(region->hasValue(3));
        region->getTiles().getTail()->getElement()->setValue(3);
        REQUIRE(region->hasValue(3));
    }

    SECTION("Test isCompleted")
    {
        REQUIRE_FALSE(region->isCompleted());
        for (const auto& tile : *region)
        {
            tile->setValue(1);
        }
        REQUIRE(region->isCompleted());
    }
}
