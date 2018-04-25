#pragma once
#include <string>
#include <memory>
#include <vector>
#include <type_traits>
#include <sstream>
#include <stdexcept>


namespace gem {


// To be implemented by the client
template<typename ValueType>
struct data_type;


class data_error : public std::runtime_error {
public:
    explicit
    data_error(const std::string& message) : std::runtime_error{message} {}
};


class data {
public:

    data(int type, std::string name)
    : type_{type}, name_{std::move(name)}
    {}

    virtual ~data() = default;

    int type() const
    {
        return type_;
    }

    const std::string& name() const
    {
        return name_;
    }

private:
    int type_;
    std::string name_;
};


class observer
{
public:
    virtual ~observer() = default;
    virtual void on_value_changed(const std::shared_ptr<gem::data>& value) = 0;
};


template<typename ValueType>
class value : public gem::data, public std::enable_shared_from_this<value<ValueType>>
{
public:

    using value_type = ValueType;

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    value(std::string name, T&& value)
    : gem::data{gem::data_type<ValueType>::value, std::move(name)}
    , value_{std::forward<T>(value)}
    {}

    // delete copy/move semantics
    value(const value&) = delete;
    value& operator=(const value&) = delete;
    value(value&&) = delete;
    value& operator=(value&&) = delete;

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    void set(T&& value)
    {
        value_ = std::forward<T>(value);
        for (const auto& observer : observers_) {
            auto obs = observer.lock();
            if (obs) {
                obs->on_value_changed(this->shared_from_this());
            }
        }
    }

    const ValueType& get() const
    {
        return value_;
    }

    void add_observer(std::weak_ptr<gem::observer> observer)
    {
        observers_.push_back(std::move(observer));
    }

    void remove_observer(const std::weak_ptr<gem::observer>& observer)
    {
        const auto it = std::find(observers_.begin(), observers_.end(), observer);
        if (it == observers_.end()) {
            throw gem::data_error{"No such observer"};
        }
        observers_.erase(it);
    }

private:
    ValueType value_;
    std::vector<std::weak_ptr<gem::observer>> observers_;
};


template<typename ValueType>
std::shared_ptr<gem::value<std::decay_t<ValueType>>> make_value(std::string name, ValueType&& value)
{
    return std::make_shared<gem::value<std::decay_t<ValueType>>>(std::move(name), std::forward<ValueType>(value));
}


template<typename ValueType>
std::shared_ptr<gem::value<ValueType>> cast_value(std::shared_ptr<data> value)
{
    if (data_type<ValueType>::value != value->type()) {
        throw gem::data_error{"Bad cast: ValueType does not fit value's data type"};
    }
    return std::dynamic_pointer_cast<gem::value<ValueType>>(value);
}


template<typename ValueType>
std::string to_string(const std::shared_ptr<gem::value<ValueType>>& value, char delim='|')
{
    if (data_type<ValueType>::value != value->type()) {
        throw gem::data_error{"ValueType does not match value's data type"};
    }
    return std::to_string(value->type()) + std::string(1, delim)
           + value->name() + std::string(1, delim)
           + gem::data_type<ValueType>::to_string(value->get());
}


template<typename ValueType>
std::shared_ptr<gem::value<ValueType>> from_string(const std::string& data, char delim='|')
{
    std::stringstream ss(data);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    if (tokens.size() != 3) {
        throw gem::data_error{"Expect exactly three elements in string"};
    }
    auto value = gem::make_value(tokens[1], gem::data_type<ValueType>::from_string(tokens[2]));
    if (std::atoi(tokens[0].c_str()) != value->type()) {
        throw gem::data_error{"ValueType does not match data type found in string"};
    }
    return value;
}


} // gem


#define GEM_VALUE(name, value) \
auto name = gem::make_value(#name, value)
