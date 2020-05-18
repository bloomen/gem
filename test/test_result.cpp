#include <gem/result.h>

#include "catch.hpp"

namespace
{

gem::Result<double> success_func()
{
    return 13.3;
}

gem::Result<void> success_func_void()
{
    return {};
}

gem::Result<double> fail_func()
{
    return gem::Error{"rough day"};
}

gem::Result<void> fail_func_void()
{
    return gem::Error{"rough day again"};
}

}

TEST_CASE("result") {
    auto res1 = success_func();
    REQUIRE(res1.ok());
    REQUIRE(13.3 == res1.value());
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
    REQUIRE(13.3 == res.unwrap());
    REQUIRE_THROWS_AS(fail_func().unwrap(), gem::ResultError);
    auto res2 = fail_func();
    REQUIRE_THROWS_AS(res2.unwrap(), gem::ResultError);
}

TEST_CASE("result_match_success") {
    success_func().match(
                [](double x){ REQUIRE(13.3 == x); },
                [](auto){ REQUIRE(false); }
            );
    auto res = success_func();
    res.match(
        [](double x){ REQUIRE(13.3 == x); },
        [](auto){ REQUIRE(false); }
    );
    res.match(
        [](double x){ REQUIRE(13.3 == x); },
        [](auto){ REQUIRE(false); }
    );
    std::move(res).match(
        [](double x){ REQUIRE(13.3 == x); },
        [](auto){ REQUIRE(false); }
    );
}

TEST_CASE("result_match_error") {
    fail_func().match(
                [](double){ REQUIRE(false); },
                [](auto e){ REQUIRE("rough day" == e.message()); }
            );
    auto res = fail_func();
    res.match(
        [](double){ REQUIRE(false); },
        [](auto e){ REQUIRE("rough day" == e.message()); }
    );
}

TEST_CASE("result_void") {
    auto res1 = success_func_void();
    REQUIRE(res1.ok());
    auto res2 = fail_func_void();
    REQUIRE(!res2.ok());
    REQUIRE("rough day again" == res2.error().message());
}

TEST_CASE("result_void_unwrap") {
    success_func().unwrap();
    REQUIRE_THROWS_AS(fail_func().unwrap(), gem::ResultError);
}

TEST_CASE("result_void_match_success") {
    success_func().match(
                [](double){ REQUIRE(true); },
                [](auto){ REQUIRE(false); }
            );
}

TEST_CASE("result_void_match_error") {
    fail_func().match(
                [](double){ REQUIRE(false); },
                [](auto){ REQUIRE(true); }
            );
}
