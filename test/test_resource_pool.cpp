#include <gem/resource_pool.h>

#include "catch.hpp"

TEST_CASE("allocate")
{
    gem::resource_pool mm;
    auto p = mm.allocate<int>(42);
    REQUIRE(42 == *p);

    //    std::weak_ptr<int> w = p;
}

TEST_CASE("garbage_collect")
{
    gem::resource_pool mm;
    {
        auto p = mm.allocate<int>(42);
        REQUIRE(42 == *p);
    }
    mm.garbage_collect();
}
