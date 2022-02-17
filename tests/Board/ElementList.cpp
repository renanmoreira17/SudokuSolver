#include <catch2/catch_test_macros.hpp>

#include "Board/ElementList.hpp"

TEST_CASE("Test ElementList creation", "[ElementList]")
{
    int firstMocInt = 1;
    int middleMocInt = 2;
    int lastMocInt = 3;
    auto* first = new ElementWrapper<int>(&firstMocInt);
    auto* middle = new ElementWrapper<int>(&middleMocInt, first);
    auto* last = new ElementWrapper<int>(&lastMocInt, middle);

    SECTION("Create element list from single element wrapper - head")
    {
        ElementList<int> eListFirst(first);
        REQUIRE(eListFirst.getHead() == first);
        REQUIRE(eListFirst.getTail() == last);
    }

    SECTION("Create element list from single element wrapper - middle")
    {
        ElementList<int> eListMiddle(middle);
        REQUIRE(eListMiddle.getHead() == first);
        REQUIRE(eListMiddle.getTail() == last);
    }

    SECTION("Create element list from single element wrapper - tail")
    {
        ElementList<int> eListLast(last);
        REQUIRE(eListLast.getHead() == first);
        REQUIRE(eListLast.getTail() == last);
    }

    SECTION("Create element list from head and tail element wrapper")
    {
        ElementList<int> eList(first, last);
        REQUIRE(eList.getHead() == first);
        REQUIRE(eList.getTail() == last);
        REQUIRE(eList.getHead()->next() == eList.getTail()->previous());
    }
}

TEST_CASE("Test ElementList iterator", "[ElementList]")
{
    int firstMocInt = 1;
    int middleMocInt = 2;
    int lastMocInt = 3;
    auto* first = new ElementWrapper<int>(&firstMocInt);
    auto* middle = new ElementWrapper<int>(&middleMocInt, first);
    auto* last = new ElementWrapper<int>(&lastMocInt, middle);

    SECTION("Normal iterator")
    {
        ElementList<int> eList(first);
        const std::vector<ElementWrapper<int>*> golden = {first, middle, last};
        auto goldenIt = golden.begin();
        for (auto it = eList.begin(); it != eList.end(); ++it)
        {
            REQUIRE(*it == (*goldenIt)->getElement());
            REQUIRE(&(*it) == &(*goldenIt)->getElement());
            ++goldenIt;
        }
    }

    SECTION("Const iterator")
    {
        ElementList<int> eList(first);
        const std::vector<ElementWrapper<int>*> golden = {first, middle, last};
        auto goldenIt = golden.begin();
        for (auto it = eList.cbegin(); it != eList.cend(); ++it)
        {
            REQUIRE(*it == (*goldenIt)->getElement());
            REQUIRE(&(*it) == &(*goldenIt)->getElement());
            ++goldenIt;
        }
    }
}
