#include <gem/datastore.h>

#include "catch.hpp"


enum class type {
    int32,
    real32
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
        return std::atoi(data.c_str());
    }
};

template<>
struct data_type<float> {
    static constexpr int type_index = static_cast<int>(type::real32);
    static std::string to_string(float value)
    {
        return std::to_string(value);
    }
    static float from_string(const std::string& data)
    {
        return static_cast<float>(std::atof(data.c_str()));
    }
};

}
}

TEST_CASE("value") {
    auto value = gem::ds::make_value("foo", 42);
    REQUIRE(*value->get() == 42);
    REQUIRE(value->name() == "foo");
    REQUIRE(value->type() == static_cast<int>(type::int32));
}

struct mock_observer : gem::ds::observer {
    int value_{};
    std::string name_;
    void on_data_changed(const std::shared_ptr<gem::ds::data>& value) override
    {
        name_ = value->name();
        value_ = *gem::ds::cast_value<int>(value)->get();
    }
};

TEST_CASE("value_observer") {
    auto value = gem::ds::make_value("foo", 42);
    auto observer = std::make_shared<mock_observer>();
    value->add_observer(observer);
    value->set(43);
    REQUIRE(43 == observer->value_);
    REQUIRE("foo" == observer->name_);
    value->remove_observer(observer);
    value->set(45);
    REQUIRE(43 == observer->value_);
    REQUIRE("foo" == observer->name_);
}

TEST_CASE("to_string") {
    auto value = gem::ds::make_value("foo", 42);
    const auto serialized = gem::ds::to_string(value);
    const std::string expected = "0|foo|42";
    REQUIRE(expected == serialized);
}

TEST_CASE("to_string_with_null_value") {
    auto value = gem::ds::make_value<int>("foo");
    const auto serialized = gem::ds::to_string(value);
    const std::string expected = "0|foo";
    REQUIRE(expected == serialized);
}

TEST_CASE("from_string") {
    const std::string serialized = "0|foo|42";
    auto value = gem::ds::from_string<int>(serialized);
    REQUIRE(*value->get() == 42);
    REQUIRE(value->name() == "foo");
    REQUIRE(value->type() == static_cast<int>(type::int32));
}

TEST_CASE("from_string_with_null_value") {
    const std::string serialized = "0|foo";
    auto value = gem::ds::from_string<int>(serialized);
    REQUIRE_FALSE(value->get());
    REQUIRE(value->name() == "foo");
    REQUIRE(value->type() == static_cast<int>(type::int32));
}

TEST_CASE("use_multiple_values") {
    GEMDS_VALUE(foo, 42);
    GEMDS_VALUE(bar, 13.3f);
    GEMDS_VALUE(albert, 17);
    REQUIRE(foo->get() == 42);
    REQUIRE(bar->get() == 13.3f);
    REQUIRE(albert->get() == 17);
}
