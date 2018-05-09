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

TEST_CASE("hashmap__put_and_size") {
    gem::hashmap<int> map;
    REQUIRE(0 == map.size());
    REQUIRE_FALSE(map.put("foo", 42));
    REQUIRE(1 == map.size());
    REQUIRE(42 == *map.put("foo", 43));
    REQUIRE(1 == map.size());
    REQUIRE_FALSE(map.put("bar", 44));
    REQUIRE(2 == map.size());
}

TEST_CASE("hashmap__put_and_get") {
    gem::hashmap<int> map;
    REQUIRE_FALSE(map.get("foo"));
    REQUIRE_FALSE(map.put("foo", 42));
    REQUIRE(42 == *map.get("foo"));
    REQUIRE(42 == *map.put("foo", 43));
    REQUIRE(43 == *map.get("foo"));
    REQUIRE_FALSE(map.put("bar", 44));
    REQUIRE(44 == *map.get("bar"));
}

TEST_CASE("hashmap__remove") {
    gem::hashmap<int> map;
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
