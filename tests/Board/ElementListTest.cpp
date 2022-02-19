#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Board/ElementList.hpp"

class ElementListModelData
{
  public:
    ElementListModelData(const int size)
    {
        for (int i = 0; i < size; ++i) { m_elements.push_back(i); }
    }

    std::vector<ElementWrapper<int>*> createWrappers()
    {
        std::vector<ElementWrapper<int>*> wrappers;
        ElementWrapper<int>* lastWrapper = nullptr;
        for (auto& element : m_elements)
        {
            auto* wrapper = new ElementWrapper<int>(&element, lastWrapper);
            wrappers.push_back(wrapper);
            lastWrapper = wrapper;
        }
        return wrappers;
    }

  private:
    std::vector<int> m_elements;
};

TEST_CASE("Test ElementList creation", "[ElementList]")
{
    ElementListModelData modelData(3);
    const auto wrappers = modelData.createWrappers();
    const auto& first = wrappers[0];
    const auto& middle = wrappers[1];
    const auto& last = wrappers[2];

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
    ElementListModelData modelData(3);
    const auto wrappers = modelData.createWrappers();
    const auto& first = wrappers[0];
    const auto& middle = wrappers[1];
    const auto& last = wrappers[2];

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

TEST_CASE("Test ElementList Copy/Move ctrs/assign operators", "[ElementList]")
{
    auto size = GENERATE(take(5, random(3, 10)));
    INFO("Size: " << size);
    ElementListModelData modelData(size);
    const auto wrappers = modelData.createWrappers();
    const auto& first = wrappers[0];

    SECTION("Copy ctr")
    {
        ElementList<int> eList(first);
        ElementList<int> eListCopy(eList);

        auto* originalPtr = eList.getHead();
        auto* copiedPtr = eListCopy.getHead();
        while (originalPtr != nullptr)
        {
            // The wrapped values must point to the same object
            REQUIRE(&originalPtr->getElement() == &copiedPtr->getElement());
            // But the wrapper must be different
            REQUIRE(originalPtr != copiedPtr);
            originalPtr = originalPtr->next();
            copiedPtr = copiedPtr->next();
        }
    }

    SECTION("Move ctr")
    {
        auto* eList = new ElementList<int>(first);
        std::vector<int*> originalPtrs;
        for (auto& e : *eList) { originalPtrs.push_back(&e); }

        ElementList<int> eListMove(std::move(*eList));
        std::vector<int*> movedPtrs;
        for (auto& e : eListMove) { movedPtrs.push_back(&e); }

        REQUIRE(originalPtrs == movedPtrs);
        REQUIRE(eList->getHead() == nullptr);
        REQUIRE(eList->getTail() == nullptr);

        delete eList;

        REQUIRE(originalPtrs == movedPtrs);
    }

    SECTION("Copy assign")
    {
        ElementList<int> eList(first);

        ElementListModelData modelDataAux(size);
        const auto wrappersAux = modelDataAux.createWrappers();
        ElementList<int> eListCopy(wrappersAux[0]);
        eListCopy = eList;

        auto* originalPtr = eList.getHead();
        auto* copiedPtr = eListCopy.getHead();
        while (originalPtr != nullptr)
        {
            // The wrapped values must point to the same object
            REQUIRE(&originalPtr->getElement() == &copiedPtr->getElement());
            // But the wrapper must be different
            REQUIRE(originalPtr != copiedPtr);
            originalPtr = originalPtr->next();
            copiedPtr = copiedPtr->next();
        }
    }

    SECTION("Move assign")
    {
        auto* eList = new ElementList<int>(first);
        std::vector<int*> originalPtrs;
        for (auto& e : *eList) { originalPtrs.push_back(&e); }

        ElementListModelData modelDataAux(size);
        const auto wrappersAux = modelDataAux.createWrappers();
        ElementList<int> eListMove(wrappersAux[0]);

        eListMove = std::move(*eList);
        std::vector<int*> movedPtrs;
        for (auto& e : eListMove) { movedPtrs.push_back(&e); }

        REQUIRE(originalPtrs == movedPtrs);
        REQUIRE(eList->getHead() == nullptr);
        REQUIRE(eList->getTail() == nullptr);

        delete eList;

        REQUIRE(originalPtrs == movedPtrs);
    }
}
