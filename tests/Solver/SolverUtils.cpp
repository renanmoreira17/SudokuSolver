#include <catch2/catch_test_macros.hpp>

#include "Solver/Solver.hpp"
#include "Solver/SolverRegions.hpp"
#include "Solver/SolverTile.hpp"
#include "Solver/SolverUtils.hpp"

TEST_CASE("Test getMutuallySeenTiles", "[SolverUtils]")
{
    Solver solver;

    SECTION("2 tiles")
    {
        SECTION("Same line")
        {
            const auto& tile1 = solver(1, 1);
            const auto& tile2 = solver(5, 1);
            const auto tiles = SolverUtils::getMutuallySeenTiles({tile1, tile2});
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverVerticalLine()->getSolverTiles();
            std::erase_if(reference, [&](const auto& tile) { return tile == tile1 || tile == tile2; });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("Same subgrid, different line")
        {
            const auto& tile1 = solver(1, 1);
            const auto& tile2 = solver(2, 2);
            const auto tiles = SolverUtils::getMutuallySeenTiles({tile1, tile2});
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverSubgrid()->getSolverTiles();
            std::erase_if(reference, [&](const auto& tile) { return tile == tile1 || tile == tile2; });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("Same subgrid, same line")
        {
            const auto& tile1 = solver(1, 1);
            const auto& tile2 = solver(2, 1);
            const auto tiles = SolverUtils::getMutuallySeenTiles({tile1, tile2});
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverVerticalLine()->getSolverTiles();
            reference.insert(reference.end(),
                             tile1->getSolverSubgrid()->getSolverTiles().begin(),
                             tile1->getSolverSubgrid()->getSolverTiles().end());
            std::erase_if(reference, [&](const auto& tile) { return tile == tile1 || tile == tile2; });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("Adjacent subgrid in diagonal")
        {
            const auto& tile1 = solver(1, 1);
            const auto& tile2 = solver(3, 3);

            const auto tiles = SolverUtils::getMutuallySeenTiles({tile1, tile2});
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            const auto reference = {solver(1, 3), solver(3, 1)};
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("Line corssing subgrid")
        {
            const auto& tile1 = solver(0, 0);
            const auto& tile2 = solver(7, 2);

            const auto tiles = SolverUtils::getMutuallySeenTiles({tile1, tile2});
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            const auto reference = {
                solver(0, 2), solver(1, 2), solver(2, 2), solver(6, 0), solver(7, 0), solver(8, 0)};
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }
    }
    SECTION("3+ tiles")
    {
        SECTION("2 in the same line, all 3 in the same subgrid")
        {
            const auto& tile1 = solver(1, 2);
            const auto& tile2 = solver(2, 2);
            const auto& tile3 = solver(0, 0);
            const auto allTiles = {tile1, tile2, tile3};
            const auto tiles = SolverUtils::getMutuallySeenTiles(allTiles);
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverSubgrid()->getSolverTiles();
            std::erase_if(reference, [&](const auto& tile) {
                return std::any_of(allTiles.begin(), allTiles.end(), [&tile](const auto& otherTile) {
                    return tile == otherTile;
                });
            });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("3 in the same line, same subgrid")
        {
            const auto& tile1 = solver(0, 2);
            const auto& tile2 = solver(1, 2);
            const auto& tile3 = solver(2, 2);
            const auto allTiles = {tile1, tile2, tile3};
            const auto tiles = SolverUtils::getMutuallySeenTiles(allTiles);
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverSubgrid()->getSolverTiles();
            reference.insert(reference.end(),
                             tile1->getSolverVerticalLine()->getSolverTiles().begin(),
                             tile1->getSolverVerticalLine()->getSolverTiles().end());
            std::erase_if(reference, [&](const auto& tile) {
                return std::any_of(allTiles.begin(), allTiles.end(), [&tile](const auto& otherTile) {
                    return tile == otherTile;
                });
            });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("3 in the same line, different subgrid")
        {
            const auto& tile1 = solver(0, 2);
            const auto& tile2 = solver(1, 2);
            const auto& tile3 = solver(3, 2);
            const auto allTiles = {tile1, tile2, tile3};
            const auto tiles = SolverUtils::getMutuallySeenTiles(allTiles);
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            auto reference = tile1->getSolverVerticalLine()->getSolverTiles();
            std::erase_if(reference, [&](const auto& tile) {
                return std::any_of(allTiles.begin(), allTiles.end(), [&tile](const auto& otherTile) {
                    return tile == otherTile;
                });
            });
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("2 in the same line, 1 in a different")
        {
            const auto& tile1 = solver(7, 8);
            const auto& tile2 = solver(8, 8);
            const auto& tile3 = solver(0, 0);
            const auto allTiles = {tile1, tile2, tile3};
            const auto tiles = SolverUtils::getMutuallySeenTiles(allTiles);
            const std::unordered_set<SolverTilePtr> setTiles(tiles.begin(), tiles.end());

            const auto reference = {solver(0, 8)};
            const std::unordered_set<SolverTilePtr> setReference(reference.begin(), reference.end());

            REQUIRE(setTiles == setReference);
        }

        SECTION("3 in unrelated tiles")
        {
            const auto& tile1 = solver(0, 0);
            const auto& tile2 = solver(5, 5);
            const auto& tile3 = solver(8, 8);
            const auto allTiles = {tile1, tile2, tile3};
            const auto tiles = SolverUtils::getMutuallySeenTiles(allTiles);
            REQUIRE(tiles.empty());
        }
    }
}
