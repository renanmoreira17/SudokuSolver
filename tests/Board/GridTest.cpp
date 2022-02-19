#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Board/Grid.hpp"
#include "Board/Line.hpp"
#include "Board/Subgrid.hpp"
#include "Board/Tile.hpp"
#include "GamesManager/Games.hpp"
#include "Util/UtilFunctions.hpp"

#include <fmt/format.h>

TEST_CASE("Test Grid creation", "[Grid]")
{
    SECTION("Check Grid creation without game string")
    {
        Grid grid;
        for (TileValueType row = 0; row < 9; ++row)
        {
            for (TileValueType col = 0; col < 9; ++col) { REQUIRE(grid(row, col)->getValue() == 0); }
        }
    }

    SECTION("Check Grid creation with game string")
    {
        auto _ = GENERATE(take(100, random(0, 0)));
        const auto& game = getGameOfDifficulty(GameDifficulty::Any);
        REQUIRE(game.size() == 81);

        Grid grid(game);
        for (TileValueType row = 0; row < 9; ++row)
        {
            for (TileValueType col = 0; col < 9; ++col)
            {
                const auto charValue = game[row * 9 + col];
                const auto value = charValue == '.' ? 0 : charValue - '0';
                REQUIRE(grid(row, col)->getValue() == value);
            }
        }
    }
}

class CustomLine : public Line
{
  public:
    CustomLine(Grid* grid, LineOrientation orientation, const short index)
        : Region(index, RegionType::LINE, grid)
        , Line(grid, orientation, index)
    {}
    CustomLine(const CustomLine& other, Grid* grid)
        : Region(other.getIndex(), other.getType(), grid)
        , Line(other, grid)
    {}
};
class CustomSubgrid : public Subgrid
{
  public:
    CustomSubgrid(Grid* grid, const short index)
        : Region(index, RegionType::SUBGRID, grid)
        , Subgrid(grid, index)
    {}
    CustomSubgrid(const CustomSubgrid& other, Grid* grid)
        : Region(other.getIndex(), other.getType(), grid)
        , Subgrid(other, grid)
    {}
};
class CustomTile : public Tile
{
  public:
    CustomTile(Grid* grid, const Coordinates& coordinates)
        : Tile(grid, coordinates)
    {}
    CustomTile(const CustomTile& other, Grid* grid)
        : Tile(other, grid)
    {}
};

class CustomComponentConstructor : public ComponentsConstructor
{
  public:
    std::shared_ptr<Tile> createTile(Grid* grid, TileValueType row, TileValueType col) const override
    {
        return std::make_shared<CustomTile>(grid, Coordinates{row, col});
    }

    std::shared_ptr<Line> createLine(Grid* grid, LineOrientation orientation, short index) const override
    {
        return std::make_shared<CustomLine>(grid, orientation, index);
    }

    std::shared_ptr<Subgrid> createSubgrid(Grid* grid, short index) const override
    {
        return std::make_shared<CustomSubgrid>(grid, index);
    }

    std::shared_ptr<Tile> createTileCopy(Grid* grid, const std::shared_ptr<Tile>& tile) const override
    {
        return std::make_shared<CustomTile>(*std::dynamic_pointer_cast<CustomTile>(tile), grid);
    }
    std::shared_ptr<Line> createLineCopy(Grid* grid, const std::shared_ptr<Line>& line) const override
    {
        return std::make_shared<CustomLine>(*std::dynamic_pointer_cast<CustomLine>(line), grid);
    }
    std::shared_ptr<Subgrid> createSubgridCopy(Grid* grid,
                                               const std::shared_ptr<Subgrid>& subgrid) const override
    {
        return std::make_shared<CustomSubgrid>(*std::dynamic_pointer_cast<CustomSubgrid>(subgrid), grid);
    }
};

TEST_CASE("Test Custom ComponentConstructor", "[Grid]")
{
    SECTION("Grid creation without game string but with custom ComponentsConstructor")
    {
        Grid grid(std::make_shared<CustomComponentConstructor>());

        SECTION("Custom Tiles")
        {
            for (TileValueType row = 0; row < 9; ++row)
            {
                for (TileValueType col = 0; col < 9; ++col)
                {
                    const auto customTile = std::dynamic_pointer_cast<CustomTile>(grid(row, col));
                    REQUIRE(customTile);
                }
            }
        }

        SECTION("Custom Lines")
        {
            for (const auto& hLine : grid.getHorizontalLines())
            {
                const auto customLine = std::dynamic_pointer_cast<CustomLine>(hLine);
                REQUIRE(customLine);
            }
            for (const auto& vLine : grid.getVerticalLines())
            {
                const auto customLine = std::dynamic_pointer_cast<CustomLine>(vLine);
                REQUIRE(customLine);
            }
        }

        SECTION("Custom Subgrids")
        {
            for (const auto& subgrid : grid.getSubgrids())
            {
                const auto customSubgrid = std::dynamic_pointer_cast<CustomSubgrid>(subgrid);
                REQUIRE(customSubgrid);
            }
        }
    }

    SECTION("Grid creation with game string and custom ComponentsConstructor")
    {
        const auto& game = getGameOfDifficulty(GameDifficulty::Any);
        REQUIRE(game.size() == 81);

        Grid grid(game, std::make_shared<CustomComponentConstructor>());

        SECTION("Custom Tiles")
        {
            for (TileValueType row = 0; row < 9; ++row)
            {
                for (TileValueType col = 0; col < 9; ++col)
                {
                    const auto charValue = game[row * 9 + col];
                    const auto value = charValue == '.' ? 0 : charValue - '0';
                    const auto customTile = std::dynamic_pointer_cast<CustomTile>(grid(row, col));
                    REQUIRE(customTile);
                    REQUIRE(customTile->getValue() == value);
                }
            }
        }

        SECTION("Custom Lines")
        {
            for (const auto& hLine : grid.getHorizontalLines())
            {
                const auto customLine = std::dynamic_pointer_cast<CustomLine>(hLine);
                REQUIRE(customLine);
            }
            for (const auto& vLine : grid.getVerticalLines())
            {
                const auto customLine = std::dynamic_pointer_cast<CustomLine>(vLine);
                REQUIRE(customLine);
            }
        }

        SECTION("Custom Subgrids")
        {
            for (const auto& subgrid : grid.getSubgrids())
            {
                const auto customSubgrid = std::dynamic_pointer_cast<CustomSubgrid>(subgrid);
                REQUIRE(customSubgrid);
            }
        }
    }
}

TEST_CASE("Test Grid copy/move ctr/operator")
{
    auto grid = GENERATE(
        Grid(),
        Grid(std::make_shared<CustomComponentConstructor>()),
        Grid(getGameOfDifficulty(GameDifficulty::Any)),
        Grid(getGameOfDifficulty(GameDifficulty::Any), std::make_shared<CustomComponentConstructor>()));

    const bool isCustom =
        std::dynamic_pointer_cast<CustomComponentConstructor>(grid.getComponentsConstructor()) != nullptr;

    SECTION("Copy Ctr")
    {
        Grid gridCopy(grid);
        SECTION("Tiles")
        {
            for (TileValueType row = 0; row < 9; ++row)
            {
                for (TileValueType col = 0; col < 9; ++col)
                {
                    const auto& originalTile = grid(row, col);
                    const auto& copiedTile = gridCopy(row, col);
                    REQUIRE(originalTile != copiedTile);
                    REQUIRE(originalTile->getValue() == copiedTile->getValue());
                    REQUIRE(originalTile->getCoordinates() == copiedTile->getCoordinates());
                    if (isCustom)
                    {
                        REQUIRE(std::dynamic_pointer_cast<CustomTile>(copiedTile) != nullptr);
                    }

                    REQUIRE(originalTile->getGrid() == &grid);
                    REQUIRE(copiedTile->getGrid() == &gridCopy);
                    REQUIRE(originalTile->getGrid() != copiedTile->getGrid());

                    REQUIRE(originalTile->getHorizontalLine() != copiedTile->getHorizontalLine());
                    REQUIRE(originalTile->getVerticalLine() != copiedTile->getVerticalLine());
                    REQUIRE(originalTile->getSubgrid() != copiedTile->getSubgrid());
                }
            }
        }

        SECTION("Regions")
        {
            for (const auto& coppiedHLine : gridCopy.getHorizontalLines())
            {
                const auto& originalHLine = grid.getHorizontalLine(coppiedHLine->getIndex());

                REQUIRE(originalHLine != coppiedHLine);
                REQUIRE(originalHLine->getIndex() == coppiedHLine->getIndex());
                REQUIRE(originalHLine->getLineOrientation() == coppiedHLine->getLineOrientation());
                REQUIRE(originalHLine->getRegionSpecificType() == coppiedHLine->getRegionSpecificType());

                REQUIRE(originalHLine->getGrid() == &grid);
                REQUIRE(coppiedHLine->getGrid() == &gridCopy);
                REQUIRE(originalHLine->getGrid() != coppiedHLine->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomLine>(coppiedHLine) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedHLine)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getHorizontalLine() == coppiedHLine.get());
                    REQUIRE(originalTile->getHorizontalLine() == originalHLine.get());
                    REQUIRE(originalTile->getHorizontalLine() != coppiedTile->getHorizontalLine());
                }
            }

            for (const auto& coppiedVLine : gridCopy.getVerticalLines())
            {
                const auto& originalVLine = grid.getVerticalLine(coppiedVLine->getIndex());

                REQUIRE(originalVLine != coppiedVLine);
                REQUIRE(originalVLine->getIndex() == coppiedVLine->getIndex());
                REQUIRE(originalVLine->getLineOrientation() == coppiedVLine->getLineOrientation());
                REQUIRE(originalVLine->getRegionSpecificType() == coppiedVLine->getRegionSpecificType());

                REQUIRE(originalVLine->getGrid() == &grid);
                REQUIRE(coppiedVLine->getGrid() == &gridCopy);
                REQUIRE(originalVLine->getGrid() != coppiedVLine->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomLine>(coppiedVLine) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedVLine)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getVerticalLine() == coppiedVLine.get());
                    REQUIRE(originalTile->getVerticalLine() == originalVLine.get());
                    REQUIRE(originalTile->getVerticalLine() != coppiedTile->getVerticalLine());
                }
            }

            for (const auto& coppiedSubgrid : gridCopy.getSubgrids())
            {
                const auto& originalSubgrid = grid.getSubgrid(coppiedSubgrid->getIndex());

                REQUIRE(originalSubgrid != coppiedSubgrid);
                REQUIRE(originalSubgrid->getIndex() == coppiedSubgrid->getIndex());
                REQUIRE(originalSubgrid->getRegionSpecificType() == coppiedSubgrid->getRegionSpecificType());

                REQUIRE(originalSubgrid->getGrid() == &grid);
                REQUIRE(coppiedSubgrid->getGrid() == &gridCopy);
                REQUIRE(originalSubgrid->getGrid() != coppiedSubgrid->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomSubgrid>(coppiedSubgrid) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedSubgrid)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getSubgrid() == coppiedSubgrid.get());
                    REQUIRE(originalTile->getSubgrid() == originalSubgrid.get());
                    REQUIRE(originalTile->getSubgrid() != coppiedTile->getSubgrid());
                }
            }
        }
    }

    SECTION("Copy operator")
    {
        Grid gridCopy;
        gridCopy = grid;
        SECTION("Tiles")
        {
            for (TileValueType row = 0; row < 9; ++row)
            {
                for (TileValueType col = 0; col < 9; ++col)
                {
                    const auto& originalTile = grid(row, col);
                    const auto& copiedTile = gridCopy(row, col);
                    REQUIRE(originalTile != copiedTile);
                    REQUIRE(originalTile->getValue() == copiedTile->getValue());
                    REQUIRE(originalTile->getCoordinates() == copiedTile->getCoordinates());
                    if (isCustom)
                    {
                        REQUIRE(std::dynamic_pointer_cast<CustomTile>(copiedTile) != nullptr);
                    }

                    REQUIRE(originalTile->getGrid() == &grid);
                    REQUIRE(copiedTile->getGrid() == &gridCopy);
                    REQUIRE(originalTile->getGrid() != copiedTile->getGrid());

                    REQUIRE(originalTile->getHorizontalLine() != copiedTile->getHorizontalLine());
                    REQUIRE(originalTile->getVerticalLine() != copiedTile->getVerticalLine());
                    REQUIRE(originalTile->getSubgrid() != copiedTile->getSubgrid());
                }
            }
        }

        SECTION("Regions")
        {
            for (const auto& coppiedHLine : gridCopy.getHorizontalLines())
            {
                const auto& originalHLine = grid.getHorizontalLine(coppiedHLine->getIndex());

                REQUIRE(originalHLine != coppiedHLine);
                REQUIRE(originalHLine->getIndex() == coppiedHLine->getIndex());
                REQUIRE(originalHLine->getLineOrientation() == coppiedHLine->getLineOrientation());
                REQUIRE(originalHLine->getRegionSpecificType() == coppiedHLine->getRegionSpecificType());

                REQUIRE(originalHLine->getGrid() == &grid);
                REQUIRE(coppiedHLine->getGrid() == &gridCopy);
                REQUIRE(originalHLine->getGrid() != coppiedHLine->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomLine>(coppiedHLine) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedHLine)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getHorizontalLine() == coppiedHLine.get());
                    REQUIRE(originalTile->getHorizontalLine() == originalHLine.get());
                    REQUIRE(originalTile->getHorizontalLine() != coppiedTile->getHorizontalLine());
                }
            }

            for (const auto& coppiedVLine : gridCopy.getVerticalLines())
            {
                const auto& originalVLine = grid.getVerticalLine(coppiedVLine->getIndex());

                REQUIRE(originalVLine != coppiedVLine);
                REQUIRE(originalVLine->getIndex() == coppiedVLine->getIndex());
                REQUIRE(originalVLine->getLineOrientation() == coppiedVLine->getLineOrientation());
                REQUIRE(originalVLine->getRegionSpecificType() == coppiedVLine->getRegionSpecificType());

                REQUIRE(originalVLine->getGrid() == &grid);
                REQUIRE(coppiedVLine->getGrid() == &gridCopy);
                REQUIRE(originalVLine->getGrid() != coppiedVLine->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomLine>(coppiedVLine) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedVLine)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getVerticalLine() == coppiedVLine.get());
                    REQUIRE(originalTile->getVerticalLine() == originalVLine.get());
                    REQUIRE(originalTile->getVerticalLine() != coppiedTile->getVerticalLine());
                }
            }

            for (const auto& coppiedSubgrid : gridCopy.getSubgrids())
            {
                const auto& originalSubgrid = grid.getSubgrid(coppiedSubgrid->getIndex());

                REQUIRE(originalSubgrid != coppiedSubgrid);
                REQUIRE(originalSubgrid->getIndex() == coppiedSubgrid->getIndex());
                REQUIRE(originalSubgrid->getRegionSpecificType() == coppiedSubgrid->getRegionSpecificType());

                REQUIRE(originalSubgrid->getGrid() == &grid);
                REQUIRE(coppiedSubgrid->getGrid() == &gridCopy);
                REQUIRE(originalSubgrid->getGrid() != coppiedSubgrid->getGrid());

                if (isCustom)
                {
                    REQUIRE(std::dynamic_pointer_cast<CustomSubgrid>(coppiedSubgrid) != nullptr);
                }

                for (const auto& coppiedTile : *coppiedSubgrid)
                {
                    const auto& originalTile = grid(coppiedTile->getCoordinates());

                    REQUIRE(coppiedTile->getSubgrid() == coppiedSubgrid.get());
                    REQUIRE(originalTile->getSubgrid() == originalSubgrid.get());
                    REQUIRE(originalTile->getSubgrid() != coppiedTile->getSubgrid());
                }
            }
        }
    }

    SECTION("Move Constructor")
    {
        Grid gridMoved(std::move(grid));
        if (isCustom)
        {
            REQUIRE(std::dynamic_pointer_cast<CustomComponentConstructor>(
                        gridMoved.getComponentsConstructor()) != nullptr);
        }
        SECTION("Tiles")
        {
            for (const auto& tile : gridMoved.getGridTiles()) { REQUIRE(tile->getGrid() == &gridMoved); }
        }
        SECTION("Regions")
        {
            for (const auto& hLine : gridMoved.getHorizontalLines())
            {
                REQUIRE(hLine->getGrid() == &gridMoved);
                REQUIRE(hLine->getLineOrientation() == LineOrientation::HORIZONTAL);
                const auto rowRef = hLine->getIndex();
                for (const auto& tile : *hLine)
                {
                    REQUIRE(tile->getHorizontalLine() == hLine.get());
                    REQUIRE(tile->getCoordinates().row == rowRef);
                }
            }

            for (const auto& vLine : gridMoved.getVerticalLines())
            {
                REQUIRE(vLine->getGrid() == &gridMoved);
                REQUIRE(vLine->getLineOrientation() == LineOrientation::VERTICAL);
                const auto colRef = vLine->getIndex();
                for (const auto& tile : *vLine)
                {
                    REQUIRE(tile->getVerticalLine() == vLine.get());
                    REQUIRE(tile->getCoordinates().col == colRef);
                }
            }

            for (const auto& subgrid : gridMoved.getSubgrids())
            {
                REQUIRE(subgrid->getGrid() == &gridMoved);
                const auto subgridRef = subgrid->getIndex();

                unsigned int tileRef = 0;
                for (const auto& tile : *subgrid)
                {
                    const auto refCoord = transformSubgridIndexedToAbsoluteCoordinates(tileRef++, subgridRef);
                    REQUIRE(tile->getSubgrid() == subgrid.get());
                    REQUIRE((tile->getCoordinates() == refCoord));
                }
            }
        }
    }

    SECTION("Move Operator")
    {
        Grid gridMoved;
        gridMoved = std::move(grid);
        if (isCustom)
        {
            REQUIRE(std::dynamic_pointer_cast<CustomComponentConstructor>(
                        gridMoved.getComponentsConstructor()) != nullptr);
        }
        SECTION("Tiles")
        {
            for (const auto& tile : gridMoved.getGridTiles()) { REQUIRE(tile->getGrid() == &gridMoved); }
        }
        SECTION("Regions")
        {
            for (const auto& hLine : gridMoved.getHorizontalLines())
            {
                REQUIRE(hLine->getGrid() == &gridMoved);
                REQUIRE(hLine->getLineOrientation() == LineOrientation::HORIZONTAL);
                const auto rowRef = hLine->getIndex();
                for (const auto& tile : *hLine)
                {
                    REQUIRE(tile->getHorizontalLine() == hLine.get());
                    REQUIRE(tile->getCoordinates().row == rowRef);
                }
            }

            for (const auto& vLine : gridMoved.getVerticalLines())
            {
                REQUIRE(vLine->getGrid() == &gridMoved);
                REQUIRE(vLine->getLineOrientation() == LineOrientation::VERTICAL);
                const auto colRef = vLine->getIndex();
                for (const auto& tile : *vLine)
                {
                    REQUIRE(tile->getVerticalLine() == vLine.get());
                    REQUIRE(tile->getCoordinates().col == colRef);
                }
            }

            for (const auto& subgrid : gridMoved.getSubgrids())
            {
                REQUIRE(subgrid->getGrid() == &gridMoved);
                const auto subgridRef = subgrid->getIndex();

                INFO("Subgrid: " << subgridRef);

                unsigned int tileRef = 0;
                for (const auto& tile : *subgrid)
                {
                    const auto refCoord = transformSubgridIndexedToAbsoluteCoordinates(tileRef++, subgridRef);

                    REQUIRE(tile->getSubgrid() == subgrid.get());
                    REQUIRE(tile->getCoordinates() == refCoord);
                }
            }
        }
    }
}
