#pragma once
#include "datastore.h"
#include <array>
#include <optional>
#include <string>

namespace gem
{
namespace detail
{

template <typename ValueType>
struct node
{
    std::optional<gem::ds::value<ValueType>> value;
    node* next = nullptr;

    node() = default;

    template <
        typename T,
        typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, ValueType>>>
    node(std::string key, T&& value)
        : value{std::make_optional<gem::ds::value<ValueType>>(
              std::move(key),
              std::forward<T>(value))}
    {
    }
};

} // namespace detail

template <typename ValueType, std::size_t Buckets = 0x10000>
class hashmap
{
public:
    static_assert(Buckets > 0, "Buckets must be larger than zero");

    using value_type = ValueType;

    hashmap() = default;

    ~hashmap()
    {
        destroy();
    }

    hashmap(const hashmap& other)
    {
        copy_from(other);
    }

    hashmap&
    operator=(const hashmap& other)
    {
        if (this != &other)
        {
            destroy();
            reset();
            copy_from(other);
        }
        return *this;
    }

    hashmap(hashmap&& other)
    {
        move_from(std::move(other));
    }

    hashmap&
    operator=(hashmap&& other)
    {
        if (this != &other)
        {
            destroy();
            reset();
            move_from(std::move(other));
        }
        return *this;
    }

    std::size_t
    size() const
    {
        return size_;
    }

    template <typename T,
              typename =
                  std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    std::optional<value_type>
    put(std::string key, T&& value)
    {
        auto& b = bucket(key);
        if (!b)
        {
            b = new gem::detail::node<value_type>{}; // creating head
            b->next = new gem::detail::node<value_type>{std::move(key),
                                                        std::forward<T>(value)};
            size_++;
            return {};
        }
        auto n = b->next;
        for (;;)
        {
            if (n->value->name() == key)
            {
                auto old_value = n->value->get();
                n->value->set(std::forward<T>(value));
                return old_value;
            }
            if (!n->next)
            {
                break;
            }
            n = n->next;
        };
        n->next = new gem::detail::node<value_type>{std::move(key),
                                                    std::forward<T>(value)};
        size_++;
        return {};
    }

    const std::optional<value_type>&
    get(const std::string& key) const
    {
        auto b = bucket(key);
        if (!b)
        {
            return empty_;
        }
        b = b->next;
        while (b)
        {
            if (b->value->name() == key)
            {
                return b->value->get();
            }
            b = b->next;
        };
        return empty_;
    }

    std::optional<value_type>
    remove(const std::string& key)
    {
        auto& b = bucket(key);
        if (!b)
        {
            return {};
        }
        auto p = b;
        auto n = b->next;
        while (n)
        {
            if (n->value->name() == key)
            {
                auto old_value = n->value->get();
                if (n == b->next && !n->next)
                {
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
    void
    copy_from(const hashmap& other)
    {
        for (auto b : other.buckets_)
        {
            if (b)
            {
                b = b->next;
                while (b)
                {
                    put(b->value->name(), *b->value->get());
                    b = b->next;
                }
            }
        }
    }

    void
    move_from(hashmap&& other)
    {
        std::swap(buckets_, other.buckets_);
        std::swap(size_, other.size_);
    }

    void
    destroy()
    {
        for (auto b : buckets_)
        {
            while (b)
            {
                auto trash = b;
                b = b->next;
                delete trash;
            }
        }
    }

    void
    reset()
    {
        buckets_ = {};
        size_ = 0;
    }

    gem::detail::node<value_type>*&
    bucket(const std::string& key) const
    {
        return buckets_[index(key)];
    }

    static constexpr std::size_t buckets_minus_one = Buckets - 1;

    std::size_t
    index(const std::string& key) const
    {
        return std::hash<std::string>{}(key)&buckets_minus_one;
    }

    std::optional<value_type> empty_;
    mutable std::array<gem::detail::node<value_type>*, Buckets> buckets_ = {};
    std::size_t size_ = 0;
};

} // namespace gem
