#pragma once
#include <string>
#include <memory>
#include <list>
#include <vector>
#include <type_traits>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <shared_mutex>


namespace gem {
namespace ds {


// data_type is to be implemented by the client for each supported type
template<typename ValueType>
struct data_type;


class data_error : public std::runtime_error
{
public:
    explicit
    data_error(const std::string& message)
    : std::runtime_error{message} {}
};


class data;


class observer
{
public:
    observer() = default;
    virtual ~observer() = default;

    // default copy/move semantics
    observer(const observer&) = default;
    observer& operator=(const observer&) = default;
    observer(observer&&) = default;
    observer& operator=(observer&&) = default;

    virtual void on_data_changed(const std::shared_ptr<gem::ds::data>& value) = 0;
};


class data : public std::enable_shared_from_this<data>
{
public:

    virtual ~data() = default;

    // delete copy/move semantics
    data(const data&) = delete;
    data& operator=(const data&) = delete;
    data(data&&) = delete;
    data& operator=(data&&) = delete;

    int type() const
    {
        return type_;
    }

    const std::string& name() const
    {
        return name_;
    }

    void add_observer(std::weak_ptr<gem::ds::observer> observer)
    {
        std::lock_guard lock{obs_mutex_};
        observers_.push_back(std::move(observer));
    }

    void remove_observer(const std::weak_ptr<gem::ds::observer>& observer)
    {
        if (const auto obs = observer.lock()) {
            std::lock_guard lock{obs_mutex_};
            for (auto it=observers_.cbegin(); it!=observers_.cend();) {
                const auto obs_it = it->lock();
                if (obs_it && obs_it == obs) {
                    it = observers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

protected:

    data(int type, std::string name)
    : type_{type}, name_{std::move(name)}
    {}

    void data_changed()
    {
        decltype(observers_) observers;
        {
            std::shared_lock lock{obs_mutex_};
            observers = observers_;
        }
        for (const auto& observer : observers) {
            if (auto obs = observer.lock()) {
                obs->on_data_changed(this->shared_from_this());
            }
        }
        take_out_trash();
    }

private:

    void take_out_trash()
    {
        std::lock_guard lock{obs_mutex_};
        for (auto it=observers_.cbegin(); it!=observers_.cend();) {
            if (it->expired()) {
                it = observers_.erase(it);
            } else {
                ++it;
            }
        }
    }

    int type_;
    std::string name_;
    std::list<std::weak_ptr<gem::ds::observer>> observers_;
    std::shared_mutex obs_mutex_;
};


template<typename ValueType>
class value : public gem::ds::data
{
public:

    using value_type = ValueType;

    explicit
    value(std::string name)
    : gem::ds::data{gem::ds::data_type<value_type>::type_index, std::move(name)}
    {}

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    value(std::string name, T&& value)
    : gem::ds::data{gem::ds::data_type<ValueType>::type_index, std::move(name)}
    , value_{std::forward<T>(value)}
    {}

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    void set(T&& value)
    {
        {
            std::lock_guard lock{val_mutex_};
            value_ = std::forward<T>(value);
        }
        data_changed();
    }

    const std::optional<value_type>& get() const
    {
        std::shared_lock lock{val_mutex_};
        return value_;
    }

private:
    std::optional<value_type> value_;
    mutable std::shared_mutex val_mutex_;
};


template<typename ValueType>
std::shared_ptr<gem::ds::value<std::decay_t<ValueType>>> make_value(std::string name)
{
    return std::make_shared<gem::ds::value<std::decay_t<ValueType>>>(std::move(name));
}


template<typename ValueType>
std::shared_ptr<gem::ds::value<std::decay_t<ValueType>>> make_value(std::string name, ValueType&& value)
{
    return std::make_shared<gem::ds::value<std::decay_t<ValueType>>>(std::move(name), std::forward<ValueType>(value));
}


template<typename ValueType>
std::shared_ptr<gem::ds::value<ValueType>> cast_value(std::shared_ptr<data> value)
{
    if (data_type<ValueType>::type_index != value->type()) {
        throw gem::ds::data_error{"Bad cast: ValueType does not fit value's data type"};
    }
    auto casted = std::dynamic_pointer_cast<gem::ds::value<ValueType>>(value);
    if (!casted) {
        throw gem::ds::data_error{"Bad cast: Unable to cast to gem::ds::value"};
    }
    return casted;
}


template<typename ValueType>
std::string to_string(const std::shared_ptr<gem::ds::value<ValueType>>& value, const char delim='|')
{
    if (data_type<ValueType>::type_index != value->type()) {
        throw gem::ds::data_error{"ValueType does not match value's data type"};
    }
    auto str = std::to_string(value->type()) + std::string(1, delim) + value->name();
    if (value->get()) {
        str += std::string(1, delim) + gem::ds::data_type<ValueType>::to_string(*value->get());
    }
    return str;
}


template<typename ValueType>
std::shared_ptr<gem::ds::value<ValueType>> from_string(const std::string& data, const char delim='|')
{
    std::stringstream ss(data);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    if (tokens.size() != 2 && tokens.size() != 3) {
        throw gem::ds::data_error{"Expect either two or three elements in string"};
    }
    auto value = tokens.size() == 2 ?
                 gem::ds::make_value<ValueType>(tokens[1]) :
                 gem::ds::make_value(tokens[1], gem::ds::data_type<ValueType>::from_string(tokens[2]));
    if (std::atoi(tokens[0].c_str()) != value->type()) {
        throw gem::ds::data_error{"ValueType does not match data type found in string"};
    }
    return value;
}


} // ds
} // gem


#define GEMDS_VALUE(name, value) \
auto name = gem::ds::make_value(#name, value)
