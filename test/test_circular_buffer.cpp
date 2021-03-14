#include "catch.hpp"
#include <gem/circular_buffer.h>

using gem::circular_buffer;

TEST_CASE("circular_buffer__buffer_of_capacity_one_no_elements")
{
    constexpr int capacity = 1;
    circular_buffer<double, capacity> buffer;
    static_assert(capacity == buffer.capacity(), "");
    REQUIRE(0 == buffer.size());
    REQUIRE(buffer.empty());
}

TEST_CASE("circular_buffer__buffer_of_capacity_one_with_one_added")
{
    constexpr int capacity = 1;
    circular_buffer<double, capacity> buffer;
    static_assert(capacity == buffer.capacity(), "");
    const double value = 42;
    buffer.push(value);
    REQUIRE(value == buffer.front());
    REQUIRE(1 == buffer.size());
}

TEST_CASE("circular_buffer__buffer_of_capacity_one_with_two_added")
{
    constexpr int capacity = 1;
    circular_buffer<double, capacity> buffer;
    static_assert(capacity == buffer.capacity(), "");
    const double value1 = 42;
    const double value2 = 43.6;
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.size());
    buffer.push(value2);
    REQUIRE(value2 == buffer.front());
    REQUIRE(1 == buffer.size());
}

TEST_CASE("circular_buffer__buffer_of_capacity_two_with_two_added")
{
    constexpr int capacity = 2;
    circular_buffer<double, capacity> buffer;
    static_assert(capacity == buffer.capacity(), "");
    const double value1 = 42;
    const double value2 = 43.6;
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.size());
    buffer.push(value2);
    REQUIRE(value1 == buffer.front());
    REQUIRE(2 == buffer.size());
}

TEST_CASE("circular_buffer__buffer_of_capacity_two_with_three_added")
{
    constexpr int capacity = 2;
    circular_buffer<double, capacity> buffer;
    static_assert(capacity == buffer.capacity(), "");
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    REQUIRE(1 == buffer.size());
    buffer.push(value2);
    REQUIRE(value1 == buffer.front());
    REQUIRE(2 == buffer.size());
    buffer.push(value3);
    REQUIRE(value2 == buffer.front());
    REQUIRE(2 == buffer.size());
}

TEST_CASE("circular_buffer__buffer_of_capacity_one_pop")
{
    circular_buffer<double, 1> buffer;
    const double value1 = 42;
    const double value2 = 46;
    REQUIRE(buffer.empty());
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    buffer.push(value2);
    REQUIRE(value2 == buffer.front());
    buffer.pop();
    REQUIRE(buffer.empty());
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_capacity_two_pop")
{
    circular_buffer<double, 2> buffer;
    REQUIRE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
    const double value1 = 42;
    const double value2 = 46;
    buffer.push(value1);
    REQUIRE_FALSE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
    buffer.push(value2);
    REQUIRE_FALSE(buffer.empty());
    REQUIRE(buffer.full());
    REQUIRE(value1 == buffer.front());
    buffer.pop();
    REQUIRE(value2 == buffer.front());
    buffer.pop();
    REQUIRE(buffer.empty());
    REQUIRE(buffer.empty());
    REQUIRE_FALSE(buffer.full());
}

TEST_CASE("circular_buffer__buffer_of_capacity_three_with_push_overload")
{
    circular_buffer<double, 3> buffer;
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    const double value4 = 7;
    const double value5 = 8;
    const double value6 = 9;
    const double value7 = 10;
    const double value8 = 11;
    buffer.push(42.);
    REQUIRE(value1 == buffer.front());
    buffer.push(46.);
    REQUIRE(value1 == buffer.front());
    buffer.push(14.);
    REQUIRE(value1 == buffer.front());
    buffer.push(7.);
    REQUIRE(value2 == buffer.front());
    buffer.push(8.);
    REQUIRE(value3 == buffer.front());
    buffer.push(9.);
    REQUIRE(value4 == buffer.front());
    buffer.push(10.);
    REQUIRE(value5 == buffer.front());
    buffer.push(11.);
    REQUIRE(value6 == buffer.front());
    buffer.push(12.);
    REQUIRE(value7 == buffer.front());
    buffer.push(13.);
    REQUIRE(value8 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_capacity_three_push_and_pop")
{
    circular_buffer<double, 3> buffer;
    const double value1 = 42;
    const double value2 = 46;
    const double value3 = 14;
    const double value4 = 7;
    const double value5 = 8;
    const double value6 = 9;
    const double value7 = 10;
    const double value8 = 11;
    REQUIRE(buffer.empty());
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    buffer.pop();
    REQUIRE(buffer.empty());
    buffer.push(value1);
    REQUIRE(value1 == buffer.front());
    buffer.push(value2);
    REQUIRE(value1 == buffer.front());
    buffer.pop();
    REQUIRE(value2 == buffer.front());
    buffer.push(value1);
    buffer.push(value3);
    REQUIRE(value2 == buffer.front());
}

TEST_CASE("circular_buffer__buffer_of_capacity_three_push_and_pop_guanpath")
{
    circular_buffer<double, 3> buffer;
    const double value1 = 1;
    const double value2 = 2;
    const double value3 = 3;
    const double value4 = 4;
    const double value5 = 5;
    const double value6 = 6;
    const double value7 = 7;
    const double value8 = 8;
    const double value9 = 9;
    const double value10 = 10;
    const double value11 = 11;
    const double value12 = 12;

    REQUIRE(buffer.empty());
    buffer.push(value1);
    buffer.push(value2);
    buffer.push(value3);
    buffer.push(value4);
    buffer.pop();
    REQUIRE(value3 == buffer.front());
    buffer.push(value5);
    REQUIRE(value3 == buffer.front());
    buffer.pop();
    REQUIRE(value4 == buffer.front());
    buffer.pop();
    REQUIRE(value5 == buffer.front());
    buffer.pop();
    buffer.push(value8);
    REQUIRE(value8 == buffer.front());
    buffer.push(value9);
    buffer.push(value10);
    REQUIRE(value8 == buffer.front());
    buffer.pop();
    buffer.pop();

    REQUIRE(value10 == buffer.front());
}

TEST_CASE("circular_buffer__swap")
{
    circular_buffer<int, 3> buffer1;
    buffer1.push(1);
    buffer1.push(2);
    buffer1.push(3);
    REQUIRE(3 == buffer1.size());
    circular_buffer<int, 3> buffer2;
    buffer2.push(41);
    buffer2.push(42);
    REQUIRE(2 == buffer2.size());
    std::swap(buffer1, buffer2);
    REQUIRE(2 == buffer1.size());
    REQUIRE(3 == buffer2.size());
    REQUIRE(41 == buffer1.front());
    buffer1.pop();
    REQUIRE(42 == buffer1.front());
    buffer1.pop();
    REQUIRE(1 == buffer2.front());
    buffer2.pop();
    REQUIRE(2 == buffer2.front());
    buffer2.pop();
    REQUIRE(3 == buffer2.front());
    buffer2.pop();
}

TEST_CASE("circular_buffer__is_equal")
{
    circular_buffer<int, 3> buffer1;
    buffer1.push(1);
    buffer1.push(2);
    REQUIRE(buffer1 == buffer1);
    circular_buffer<int, 3> buffer2;
    buffer2.push(1);
    buffer2.push(2);
    REQUIRE(buffer2 == buffer2);
}

TEST_CASE("circular_buffer__is_not_equal")
{
    circular_buffer<int, 3> buffer1;
    buffer1.push(1);
    buffer1.push(2);
    circular_buffer<int, 3> buffer2;
    buffer2.push(1);
    buffer2.push(13);
    REQUIRE(buffer1 != buffer2);
    circular_buffer<int, 3> buffer3;
    buffer2.push(1);
    REQUIRE(buffer1 != buffer3);
    REQUIRE(buffer2 != buffer3);
}

TEST_CASE("circular_buffer__front_non_const_overload")
{
    circular_buffer<int, 3> buffer1;
    buffer1.push(1);
    REQUIRE(1 == buffer1.front());
    buffer1.front() = 42;
    REQUIRE(42 == buffer1.front());
    REQUIRE(1 == buffer1.size());
}
