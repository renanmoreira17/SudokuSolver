#include <catch2/catch_test_macros.hpp>

#include "Board/ElementWrapper.hpp"

TEST_CASE("Test Element wrapper creation", "[ElementWrapper]")
{
    int mocInt = 1;
    SECTION("Test single ElementWrapper creation")
    {
        ElementWrapper<int> e(&mocInt);
        CHECK(&e.getElement() == &mocInt);
        REQUIRE(e.getElement() == mocInt);
        REQUIRE_FALSE(e.hasNext());
        REQUIRE_FALSE(e.hasPrevious());
        CHECK(e.next() == nullptr);
        CHECK(e.previous() == nullptr);
    }

    SECTION("Test single LineElementWrapper creation")
    {
        LineElementWrapper<int> e(&mocInt);
        CHECK(&e.getElement() == &mocInt);
        REQUIRE(e.getElement() == mocInt);
        REQUIRE_FALSE(e.hasNext());
        REQUIRE_FALSE(e.hasPrevious());
        CHECK(e.next() == nullptr);
        CHECK(e.previous() == nullptr);
    }

    SECTION("Test single SubgridElementWrapper creation")
    {
        SubgridElementWrapper<int> e(&mocInt);
        CHECK(&e.getElement() == &mocInt);
        REQUIRE(e.getElement() == mocInt);
        REQUIRE_FALSE(e.hasNext());
        REQUIRE_FALSE(e.hasPrevious());
        CHECK(e.next() == nullptr);
        CHECK(e.previous() == nullptr);
    }
}

TEST_CASE("Test chained Element wrapper creation", "[ElementWrapper]")
{
    int firstMocInt = 1;
    int middleMocInt = 2;
    int lastMocInt = 3;
    SECTION("Test chained ElementWrapper creation")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        CHECK(&first.getElement() == &firstMocInt);
        REQUIRE(first.getElement() == firstMocInt);
        REQUIRE_FALSE(first.hasPrevious());
        REQUIRE(first.hasNext());
        CHECK(first.previous() == nullptr);
        REQUIRE(first.next() == &middle);

        CHECK(&middle.getElement() == &middleMocInt);
        REQUIRE(middle.getElement() == middleMocInt);
        REQUIRE(middle.hasPrevious());
        REQUIRE(middle.hasNext());
        REQUIRE(middle.previous() == &first);
        REQUIRE(middle.next() == &last);

        CHECK(&last.getElement() == &lastMocInt);
        REQUIRE(last.getElement() == lastMocInt);
        REQUIRE(last.hasPrevious());
        REQUIRE_FALSE(last.hasNext());
        REQUIRE(last.previous() == &middle);
        CHECK(last.next() == nullptr);
    }

    SECTION("Test chained LineElementWrapper creation")
    {
        LineElementWrapper<int> first(&firstMocInt);
        LineElementWrapper<int> middle(&middleMocInt, &first);
        LineElementWrapper<int> last(&lastMocInt, &middle);

        CHECK(&first.getElement() == &firstMocInt);
        REQUIRE(first.getElement() == firstMocInt);
        REQUIRE_FALSE(first.hasPrevious());
        REQUIRE(first.hasNext());
        CHECK(first.previous() == nullptr);
        REQUIRE(first.next() == &middle);

        CHECK(&middle.getElement() == &middleMocInt);
        REQUIRE(middle.getElement() == middleMocInt);
        REQUIRE(middle.hasPrevious());
        REQUIRE(middle.hasNext());
        REQUIRE(middle.previous() == &first);
        REQUIRE(middle.next() == &last);

        CHECK(&last.getElement() == &lastMocInt);
        REQUIRE(last.getElement() == lastMocInt);
        REQUIRE(last.hasPrevious());
        REQUIRE_FALSE(last.hasNext());
        REQUIRE(last.previous() == &middle);
        CHECK(last.next() == nullptr);
    }

    SECTION("Test chained SubgridElementWrapper creation")
    {
        SubgridElementWrapper<int> first(&firstMocInt);
        SubgridElementWrapper<int> middle(&middleMocInt, &first);
        SubgridElementWrapper<int> last(&lastMocInt, &middle);

        CHECK(&first.getElement() == &firstMocInt);
        REQUIRE(first.getElement() == firstMocInt);
        REQUIRE_FALSE(first.hasPrevious());
        REQUIRE(first.hasNext());
        CHECK(first.previous() == nullptr);
        REQUIRE(first.next() == &middle);

        CHECK(&middle.getElement() == &middleMocInt);
        REQUIRE(middle.getElement() == middleMocInt);
        REQUIRE(middle.hasPrevious());
        REQUIRE(middle.hasNext());
        REQUIRE(middle.previous() == &first);
        REQUIRE(middle.next() == &last);

        CHECK(&last.getElement() == &lastMocInt);
        REQUIRE(last.getElement() == lastMocInt);
        REQUIRE(last.hasPrevious());
        REQUIRE_FALSE(last.hasNext());
        REQUIRE(last.previous() == &middle);
        CHECK(last.next() == nullptr);
    }
}

TEST_CASE("Test Move ctrs/assign operator")
{
    int firstMocInt = 1;
    int middleMocInt = 2;
    int lastMocInt = 3;

    SECTION("Move ctr - first")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        ElementWrapper<int> firstMoved(std::move(first));

        REQUIRE(&firstMoved.getElement() == &firstMocInt);
        REQUIRE(firstMoved.previous() == nullptr);
        REQUIRE(firstMoved.next() == &middle);

        REQUIRE(first.getElementPtr() == nullptr);
        REQUIRE(first.previous() == nullptr);
        REQUIRE(first.next() == nullptr);
    }

    SECTION("Move ctr - middle")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        ElementWrapper<int> middleMoved(std::move(middle));

        REQUIRE(&middleMoved.getElement() == &middleMocInt);
        REQUIRE(middleMoved.previous() == &first);
        REQUIRE(middleMoved.next() == &last);

        REQUIRE(middle.getElementPtr() == nullptr);
        REQUIRE(middle.previous() == nullptr);
        REQUIRE(middle.next() == nullptr);
    }

    SECTION("Move ctr - last")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        ElementWrapper<int> lastMoved(std::move(last));

        REQUIRE(&lastMoved.getElement() == &lastMocInt);
        REQUIRE(lastMoved.previous() == &middle);
        REQUIRE(lastMoved.next() == nullptr);

        REQUIRE(last.getElementPtr() == nullptr);
        REQUIRE(last.previous() == nullptr);
        REQUIRE(last.next() == nullptr);
    }

    SECTION("Move assign - first")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        int auxInt = 0;
        ElementWrapper<int> firstMoved(&auxInt);
        firstMoved = std::move(first);

        REQUIRE(&firstMoved.getElement() == &firstMocInt);
        REQUIRE(firstMoved.previous() == nullptr);
        REQUIRE(firstMoved.next() == &middle);

        REQUIRE(first.getElementPtr() == nullptr);
        REQUIRE(first.previous() == nullptr);
        REQUIRE(first.next() == nullptr);
    }

    SECTION("Move assign - middle")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        int auxInt = 0;
        ElementWrapper<int> middleMoved(&auxInt);
        middleMoved = std::move(middle);

        REQUIRE(&middleMoved.getElement() == &middleMocInt);
        REQUIRE(middleMoved.previous() == &first);
        REQUIRE(middleMoved.next() == &last);

        REQUIRE(middle.getElementPtr() == nullptr);
        REQUIRE(middle.previous() == nullptr);
        REQUIRE(middle.next() == nullptr);
    }

    SECTION("Move assign - last")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        int auxInt = 0;
        ElementWrapper<int> lastMoved(&auxInt);
        lastMoved = std::move(last);

        REQUIRE(&lastMoved.getElement() == &lastMocInt);
        REQUIRE(lastMoved.previous() == &middle);
        REQUIRE(lastMoved.next() == nullptr);

        REQUIRE(last.getElementPtr() == nullptr);
        REQUIRE(last.previous() == nullptr);
        REQUIRE(last.next() == nullptr);
    }

    SECTION("Move assign - itself")
    {
        ElementWrapper<int> first(&firstMocInt);
        ElementWrapper<int> middle(&middleMocInt, &first);
        ElementWrapper<int> last(&lastMocInt, &middle);

        middle = std::move(middle);

        REQUIRE(&middle.getElement() == &middleMocInt);
        REQUIRE(middle.previous() == &first);
        REQUIRE(middle.next() == &last);
    }
}
