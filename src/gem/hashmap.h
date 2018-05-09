#pragma once
#include <optional>
#include <string>
#include <array>
#include "datastore.h"


namespace gem {
namespace detail {

template<typename ValueType>
struct node
{
    gem::ds::value<ValueType> value;
    node* next = nullptr;

    node()
    : value{""}
    {}

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, ValueType>>>
    node(std::string key, T&& value)
    : value{std::move(key), std::forward<T>(value)}
    {}
};

}


template<typename ValueType>
class hashmap
{
public:

    using value_type = ValueType;

    hashmap() = default;

    ~hashmap()
    {
        for (auto b : buckets_) {
            while (b) {
                auto trash = b;
                b = b->next;
                delete trash;
            }
        }
    }

    std::size_t
    size() const
    {
        return size_;
    }

    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    std::optional<value_type>
    put(std::string key, T&& value)
    {
        auto& b = bucket(key);
        if (!b) {
            b = new gem::detail::node<value_type>{}; // creating head
            b->next = new gem::detail::node<value_type>{std::move(key), std::forward<T>(value)};
            size_++;
            return {};
        }
        auto n = b->next;
        for (;;) {
            if (n->value.name() == key) {
                auto old_value = n->value.get();
                n->value.set(std::forward<T>(value));
                return old_value;
            }
            if (!n->next) {
                break;
            }
            n = n->next;
        };
        n->next = new gem::detail::node<value_type>{std::move(key), std::forward<T>(value)};
        size_++;
        return {};
    }

    std::optional<value_type>
    get(const std::string& key) const
    {
        auto b = bucket(key);
        if (!b) {
            return {};
        }
        b = b->next;
        while (b) {
            if (b->value.name() == key) {
                return b->value.get();
            }
            b = b->next;
        };
        return {};
    }

    std::optional<value_type>
    remove(const std::string& key)
    {
        auto& b = bucket(key);
        if (!b) {
            return {};
        }
        auto p = b;
        auto n = b->next;
        while (n) {
            if (n->value.name() == key) {
                auto old_value = n->value.get();
                if (n == b->next && !n->next) {
                    // deleting head
                    delete b;
                    b = nullptr;
                }
                p->next = n->next;
                delete n;
                size_--;
                return old_value;
            }
            p = n;
            n = n->next;
        };
        return {};
    }

private:

    gem::detail::node<value_type>*&
    bucket(const std::string& key) const
    {
        return buckets_[index(key)];
    }

    static constexpr std::size_t n_buckets = 0x10000;
    static constexpr std::size_t n_buckets_minus_one = n_buckets - 1;

    std::size_t
    index(const std::string& key) const
    {
        return std::hash<std::string>{}(key) & n_buckets_minus_one;
    }

    mutable std::array<gem::detail::node<value_type>*, n_buckets> buckets_{};
    std::size_t size_ = 0;
};


}
