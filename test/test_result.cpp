#include <gem/result.h>

#include "catch.hpp"

namespace
{

gem::Result<double> success_func()
{
    return 13.3;
}

gem::Result<double> fail_func()
{
    return gem::Error{"rough day"};
}

}


TEST_CASE("result") {
    auto res1 = success_func();
    REQUIRE(res1.ok());
    REQUIRE(13.3 == res1.data());
    auto res2 = fail_func();
    REQUIRE(!res2.ok());
    REQUIRE("rough day" == res2.error().message());
}

TEST_CASE("result_unwrap") {
    auto data = success_func().unwrap();
    REQUIRE(13.3 == data);
    auto res = success_func();
    auto d = res.unwrap();
    REQUIRE(13.3 == d);
    REQUIRE_THROWS_AS(res.unwrap(), gem::ResultError);
    REQUIRE_THROWS_AS(fail_func().unwrap(), gem::ResultError);
}
