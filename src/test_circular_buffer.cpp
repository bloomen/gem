#include "catch.hpp"
#include "circular_buffer.h"


TEST_CASE("circular_buffer__buffer_of_size_one_no_elements") {
    constexpr int size = 1;
    circular_buffer<double, size> buffer;
    static_assert(size == buffer.size(), "");
    REQUIRE(0 == buffer.populated());
    REQUIRE(0 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_size_one_with_one_added") {
    constexpr int size = 1;
    circular_buffer<double, size> buffer;
    static_assert(size == buffer.size(), "");
    const double value = 42;
    buffer.push_back(value);
    REQUIRE(value == buffer.front());
    REQUIRE(1 == buffer.populated());
}

TEST_CASE("circular_buffer__buffer_of_size_one_with_two_added") {
    constexpr int size = 1;
    circular_buffer<double, size> buffer;
    static_assert(size == buffer.size(), "");
    const double value1 = 42;
    const double value2 = 43.6;
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.populated());
    buffer.push_back(value2);
    REQUIRE(value2 == buffer.front());
    REQUIRE(1 == buffer.populated());
}

TEST_CASE("circular_buffer__buffer_of_size_two_with_two_added") {
    constexpr int size = 2;
    circular_buffer<double, size> buffer;
    static_assert(size == buffer.size(), "");
    const double value1 = 42;
    const double value2 = 43.6;
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.populated());
    buffer.push_back(value2);
    REQUIRE(value1 == buffer.front());
    REQUIRE(2 == buffer.populated());
}

TEST_CASE("circular_buffer__buffer_of_size_two_with_three_added") {
    constexpr int size = 2;
    circular_buffer<double, size> buffer;
    static_assert(size == buffer.size(), "");
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.populated());
    buffer.push_back(value2);
    REQUIRE(value1 == buffer.front());
    REQUIRE(2 == buffer.populated());
    buffer.push_back(value3);
    REQUIRE(value2 == buffer.front());
    REQUIRE(2 == buffer.populated());
}

TEST_CASE("circular_buffer__buffer_of_size_one_pop_front") {
    circular_buffer<double, 1> buffer;
    const double value1 = 42;
    const double value2 = 46;
    REQUIRE(0 == buffer.front());
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    buffer.push_back(value2);
    REQUIRE(value2 == buffer.front());
    buffer.pop_front();
    REQUIRE(0 == buffer.front());
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_size_two_pop_front") {
    circular_buffer<double, 2> buffer;
    REQUIRE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
    const double value1 = 42;
    const double value2 = 46;
    buffer.push_back(value1);
    REQUIRE_FALSE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
    buffer.push_back(value2);
    REQUIRE_FALSE(buffer.empty());
    REQUIRE(buffer.full());
    REQUIRE(value1 == buffer.front());
    buffer.pop_front();
    REQUIRE(value2 == buffer.front());
    buffer.pop_front();
    REQUIRE(0 == buffer.front());
    REQUIRE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
}

TEST_CASE("circular_buffer__buffer_of_size_three_with_push_back_overload") {
    circular_buffer<double, 3> buffer;
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    const double value4 = 7;
    const double value5 = 8;
    const double value6 = 9;
    const double value7 = 10;
    const double value8 = 11;
    buffer.push_back(42);
    REQUIRE(value1 == buffer.front());
    buffer.push_back(46);
    REQUIRE(value1 == buffer.front());
    buffer.push_back(14);
    REQUIRE(value1 == buffer.front());
    buffer.push_back(7);
    REQUIRE(value2 == buffer.front());
    buffer.push_back(8);
    REQUIRE(value3 == buffer.front());
    buffer.push_back(9);
    REQUIRE(value4 == buffer.front());
    buffer.push_back(10);
    REQUIRE(value5 == buffer.front());
    buffer.push_back(11);
    REQUIRE(value6 == buffer.front());
    buffer.push_back(12);
    REQUIRE(value7 == buffer.front());
    buffer.push_back(13);
    REQUIRE(value8 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_size_three_push_and_pop") {
    circular_buffer<double, 3> buffer;
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    const double value4 = 7;
    const double value5 = 8;
    const double value6 = 9;
    const double value7 = 10;
    const double value8 = 11;
    REQUIRE(0 == buffer.front());
    REQUIRE_FALSE(buffer.pop_front());
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(buffer.pop_front());
    REQUIRE(0 == buffer.front());
    REQUIRE_FALSE(buffer.pop_front());
    buffer.push_back(value1);
    REQUIRE(value1 == buffer.front());
    buffer.push_back(value2);
    REQUIRE(value1 == buffer.front());
    REQUIRE(buffer.pop_front());
    REQUIRE(value2 == buffer.front());
    buffer.push_back(value1);
    buffer.push_back(value3);
    REQUIRE(value2 == buffer.front());
}
