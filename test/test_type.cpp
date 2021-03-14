#include "catch.hpp"
#include <gem/type.h>

using gem::type;

TEST_CASE("type__make")
{
    const auto obj = type<int>::make(42);
    REQUIRE(!obj.is_null());
    REQUIRE(42 == obj.get());
}

TEST_CASE("type__null")
{
    const auto obj = type<int>::null();
    REQUIRE(obj.is_null());
}

TEST_CASE("type__safe_is_null")
{
    const auto obj1 = type<int>::make(42);
    REQUIRE(!obj1.safe_is_null());
    const auto obj2 = type<int>::null();
    REQUIRE(obj2.safe_is_null());
}

TEST_CASE("type__copy")
{
    const auto obj1 = type<int>::make(42);
    const auto obj2 = obj1.copy();
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
    REQUIRE(&obj1.mutex() != &obj2.mutex());
}

TEST_CASE("type__copy_inherit_mutex")
{
    const auto obj1 = type<int>::make(42);
    const auto obj2 = obj1.copy(true);
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
    REQUIRE(&obj1.mutex() == &obj2.mutex());
}

TEST_CASE("type__safe_copy")
{
    const auto obj1 = type<int>::make(42);
    const auto obj2 = obj1.safe_copy();
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
    REQUIRE(&obj1.mutex() != &obj2.mutex());
}

TEST_CASE("type__safe_copy_inherit_mutex")
{
    const auto obj1 = type<int>::make(42);
    const auto obj2 = obj1.safe_copy(true);
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
    REQUIRE(&obj1.mutex() == &obj2.mutex());
}

TEST_CASE("type__swap")
{
    auto obj1 = type<int>::make(42);
    auto obj2 = type<int>::make(13);
    obj1.swap(obj2);
    REQUIRE(!obj1.is_null());
    REQUIRE(13 == obj1.get());
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
}

TEST_CASE("type__safe_swap")
{
    auto obj1 = type<int>::make(42);
    auto obj2 = type<int>::make(13);
    obj1.safe_swap(obj2);
    REQUIRE(!obj1.is_null());
    REQUIRE(13 == obj1.get());
    REQUIRE(!obj2.is_null());
    REQUIRE(42 == obj2.get());
}

TEST_CASE("type__mutex")
{
    auto obj = type<int>::make(42);
    {
        std::lock_guard lock{obj.mutex()};
        obj.get() = 43;
    }
    {
        std::shared_lock lock{obj.mutex()};
        REQUIRE(43 == obj.get());
    }
}

TEST_CASE("type__make_with")
{
    auto mutex = std::make_shared<std::shared_mutex>();
    auto obj = type<int>::make_with(mutex, 42);
    REQUIRE(mutex.get() == &obj.mutex());
    REQUIRE(42 == obj.get());
}

TEST_CASE("type__from")
{
    auto data = std::make_shared<int>(42);
    auto obj = type<int>::from(data);
    REQUIRE(42 == obj.get());
}

TEST_CASE("type__from_with")
{
    auto mutex = std::make_shared<std::shared_mutex>();
    auto data = std::make_shared<int>(42);
    auto obj = type<int>::from_with(mutex, data);
    REQUIRE(mutex.get() == &obj.mutex());
    REQUIRE(42 == obj.get());
}

TEST_CASE("type__null_with")
{
    auto mutex = std::make_shared<std::shared_mutex>();
    auto obj = type<int>::null_with(mutex);
    REQUIRE(mutex.get() == &obj.mutex());
    REQUIRE(obj.is_null());
}
