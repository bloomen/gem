#include "catch.hpp"
#include <gem/memory_manager.h>


TEST_CASE("allocate") {
    gem::memory_manager mm;
    auto p = mm.allocate<int>(42);
    REQUIRE(42 == *p);
}

TEST_CASE("garbage_collect") {
    gem::memory_manager mm;
    {
        auto p = mm.allocate<int>(42);
        REQUIRE(42 == *p);
    }
    mm.garbage_collect();
}
