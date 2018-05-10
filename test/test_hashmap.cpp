#include "catch.hpp"
#include <gem/hashmap.h>

enum class type {
    int32,
};

namespace gem {
namespace ds {

template<>
struct data_type<std::int32_t> {
    static constexpr int type_index = static_cast<int>(type::int32);
    static std::string to_string(int value)
    {
        return std::to_string(value);
    }
    static int from_string(const std::string& data)
    {
        return std::stoi(data);
    }
};

}
}

template<std::size_t Buckets>
void test_put_and_size()
{
    gem::hashmap<int, Buckets> map;
    REQUIRE(0 == map.size());
    REQUIRE_FALSE(map.put("foo", 42));
    REQUIRE(1 == map.size());
    REQUIRE(42 == *map.put("foo", 43));
    REQUIRE(1 == map.size());
    REQUIRE_FALSE(map.put("bar", 44));
    REQUIRE(2 == map.size());
}

TEST_CASE("hashmap__put_and_size") {
    test_put_and_size<0x1>();
    test_put_and_size<0x2>();
    test_put_and_size<0x3>();
    test_put_and_size<0x10000>();
}

template<std::size_t Buckets>
void test_put_and_get()
{
    gem::hashmap<int, Buckets> map;
    REQUIRE_FALSE(map.get("foo"));
    REQUIRE_FALSE(map.put("foo", 42));
    REQUIRE(42 == *map.get("foo"));
    REQUIRE(42 == *map.put("foo", 43));
    REQUIRE(43 == *map.get("foo"));
    REQUIRE_FALSE(map.put("bar", 44));
    REQUIRE(44 == *map.get("bar"));
}

TEST_CASE("hashmap__put_and_get") {
    test_put_and_get<0x1>();
    test_put_and_get<0x2>();
    test_put_and_get<0x3>();
    test_put_and_get<0x10000>();
}

template<std::size_t Buckets>
void test_remove()
{
    gem::hashmap<int, Buckets> map;
    REQUIRE_FALSE(map.remove("foo"));
    map.put("foo", 42);
    REQUIRE(42 == *map.remove("foo"));
    REQUIRE(0 == map.size());
    map.put("foo", 42);
    map.put("bar", 43);
    REQUIRE(2 == map.size());
    REQUIRE(42 == *map.remove("foo"));
    REQUIRE(43 == *map.remove("bar"));
    REQUIRE(0 == map.size());
}

TEST_CASE("hashmap__remove") {
    test_remove<0x1>();
    test_remove<0x2>();
    test_remove<0x3>();
    test_remove<0x10000>();
}

template<std::size_t Buckets>
void test_copy_constructor()
{
    gem::hashmap<int, Buckets> map;
    map.put("foo", 42);
    map.put("bar", 43);
    gem::hashmap<int, Buckets> map2(map);
    REQUIRE(2 == map.size());
    REQUIRE(2 == map2.size());
    map.put("foo", 44);
    REQUIRE(42 == *map2.get("foo"));
    REQUIRE(43 == *map2.get("bar"));
}

TEST_CASE("hashmap__copy_constructor") {
    test_copy_constructor<0x1>();
    test_copy_constructor<0x2>();
    test_copy_constructor<0x3>();
    test_copy_constructor<0x10000>();
}

template<std::size_t Buckets>
void test_copy_assignment()
{
    gem::hashmap<int, Buckets> map;
    map.put("foo", 42);
    map.put("bar", 43);
    gem::hashmap<int, Buckets> map2;
    map2.put("bar", 44);
    map2.put("hi", 45);
    map2 = map;
    REQUIRE(2 == map.size());
    REQUIRE(2 == map2.size());
    map.put("foo", 44);
    REQUIRE(42 == *map2.get("foo"));
    REQUIRE(43 == *map2.get("bar"));
}

TEST_CASE("hashmap__copy_assignment") {
    test_copy_assignment<0x1>();
    test_copy_assignment<0x2>();
    test_copy_assignment<0x3>();
    test_copy_assignment<0x10000>();
}

template<std::size_t Buckets>
void test_move_constructor()
{
    gem::hashmap<int, Buckets> map;
    map.put("foo", 42);
    map.put("bar", 43);
    gem::hashmap<int, Buckets> map2(std::move(map));
    REQUIRE(0 == map.size());
    REQUIRE(2 == map2.size());
    REQUIRE(42 == *map2.get("foo"));
    REQUIRE(43 == *map2.get("bar"));
    REQUIRE_FALSE(map.get("foo"));
    REQUIRE_FALSE(map.get("bar"));
}

TEST_CASE("hashmap__move_constructor") {
    test_move_constructor<0x1>();
    test_move_constructor<0x2>();
    test_move_constructor<0x3>();
    test_move_constructor<0x10000>();
}

template<std::size_t Buckets>
void test_move_assignment()
{
    gem::hashmap<int, Buckets> map;
    map.put("foo", 42);
    map.put("bar", 43);
    gem::hashmap<int, Buckets> map2;
    map2 = std::move(map);
    REQUIRE(0 == map.size());
    REQUIRE(2 == map2.size());
    REQUIRE(42 == *map2.get("foo"));
    REQUIRE(43 == *map2.get("bar"));
    REQUIRE_FALSE(map.get("foo"));
    REQUIRE_FALSE(map.get("bar"));
}

TEST_CASE("hashmap__move_assignment") {
    test_move_assignment<0x1>();
    test_move_assignment<0x2>();
    test_move_assignment<0x3>();
    test_move_assignment<0x10000>();
}
