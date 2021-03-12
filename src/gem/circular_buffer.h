#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace gem
{

// A simple circular buffer (FIFO) with a capacity fixed at compile time.
// ValueType must support default construction. The buffer lets you push
// new values onto the back and pop old values off the front.
template <typename ValueType, std::size_t Capacity>
class circular_buffer
{
public:
    static_assert(
        std::is_default_constructible_v<ValueType>,
        "ValueType must be default constructible");
    static_assert(Capacity >= 1, "Capacity must be at least 1");

    using container_type = circular_buffer;
    using value_type = ValueType;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    circular_buffer() noexcept(std::is_nothrow_constructible_v<value_type>) =
        default;
    ~circular_buffer() noexcept(std::is_nothrow_destructible_v<value_type>) =
        default;

    // Default copy/move semantics
    circular_buffer(const circular_buffer&) noexcept(
        std::is_nothrow_copy_constructible_v<value_type>) = default;
    circular_buffer& operator=(const circular_buffer&) noexcept(
        std::is_nothrow_copy_assignable_v<value_type>) = default;
    circular_buffer(circular_buffer&&) noexcept(
        std::is_nothrow_move_constructible_v<value_type>) = default;
    circular_buffer& operator=(circular_buffer&&) noexcept(
        std::is_nothrow_move_assignable_v<value_type>) = default;

    // Equals operator
    bool
    operator==(const circular_buffer& other) const noexcept(
        noexcept(std::declval<value_type>() == std::declval<value_type>()))
    {
        if (this != &other)
        {
            if (size_ != other.size_)
            {
                return false;
            }
            auto front = front_;
            auto size = size_;
            while (size != 0)
            {
                if (!(data_[front] == other.data_[front]))
                {
                    return false;
                }
                increment_or_wrap(front);
                --size;
            }
            return true;
        }
        return true;
    }

    // Not-equals operator
    bool
    operator!=(const circular_buffer& other) const noexcept(
        noexcept(std::declval<value_type>() == std::declval<value_type>()))
    {
        return !(*this == other);
    }

    // Pushes a new value onto the end of the buffer. If that exceeds the
    // capacity of the buffer then the oldest value gets dropped (the one at the
    // front).
    template <
        typename T,
        typename =
            std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    void
    push(T&& value)
    {
        data_[end_] = std::forward<T>(value);
        increment();
    }

    // Returns the value at the front of the buffer (the oldest value).
    // This is undefined if the buffer is empty
    reference
    front() noexcept
    {
        return data_[front_];
    }

    // Returns the value at the front of the buffer (the oldest value).
    // This is undefined if the buffer is empty
    const_reference
    front() const noexcept
    {
        return data_[front_];
    }

    // Removes the value at the front of the buffer (the oldest value)
    void
    pop() noexcept(std::is_nothrow_constructible_v<value_type>&&
                       std::is_nothrow_destructible_v<value_type>)
    {
        if (!empty())
        {
            data_[front_].~value_type();
            decrement();
        }
    }

    // Returns the capacity of the buffer
    static constexpr size_type
    capacity() noexcept
    {
        return Capacity;
    }

    // Returns the number of populated values of the buffer. Its maximum value
    // equals the capacity of the buffer
    size_type
    size() const noexcept
    {
        return size_;
    }

    // Returns whether the buffer is empty
    bool
    empty() const noexcept
    {
        return size_ == 0;
    }

    // Returns whether the buffer is full
    bool
    full() const noexcept
    {
        return size_ == Capacity;
    }

    // Swaps this buffer with the given buffer
    template <typename = std::enable_if_t<std::is_swappable_v<value_type>>>
    void
    swap(circular_buffer& other) noexcept(
        std::is_nothrow_swappable_v<value_type>)
    {
        std::swap(end_, other.end_);
        std::swap(front_, other.front_);
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

private:
    void
    increment_or_wrap(size_type& value) const noexcept
    {
        if (value == Capacity - 1)
        {
            value = 0;
        }
        else
        {
            ++value;
        }
    }

    void
    increment() noexcept
    {
        increment_or_wrap(end_);
        if (full())
        {
            increment_or_wrap(front_);
        }
        else
        {
            ++size_;
        }
    }

    void
    decrement() noexcept
    {
        increment_or_wrap(front_);
        --size_;
    }

    size_type end_{};
    size_type front_{};
    size_type size_{};
    value_type data_[Capacity];
};

} // namespace gem

namespace std
{

template <
    typename ValueType,
    std::size_t Capacity,
    typename = std::enable_if_t<std::is_swappable_v<ValueType>>>
void
swap(
    gem::circular_buffer<ValueType, Capacity>& lhs,
    gem::circular_buffer<ValueType, Capacity>&
        rhs) noexcept(std::is_nothrow_swappable_v<ValueType>)
{
    lhs.swap(rhs);
}

} // namespace std
