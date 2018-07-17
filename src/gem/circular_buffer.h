#pragma once
#include <type_traits>
#include <cstddef>
#include <array>


namespace gem {


// A simple circular buffer with a size fixed at compile time.
// ValueType must support default construction. The buffer lets you push
// new values onto the back and pop old values off the front.
template<typename ValueType, std::size_t Capacity>
class circular_buffer
{
public:

    static_assert(std::is_default_constructible_v<ValueType>, "ValueType must be default constructible");
    static_assert(Capacity >= 1, "Size must be at least 1");

    using value_type = ValueType;

    circular_buffer() = default;

    // default copy/move semantics
    circular_buffer(const circular_buffer&) = default;
    circular_buffer& operator=(const circular_buffer&) = default;
    circular_buffer(circular_buffer&&) = default;
    circular_buffer& operator=(circular_buffer&&) = default;

    // Pushes a new value onto the end of the buffer. If that exceeds the size
    // of the buffer then the oldest value gets dropped (the one at the front).
    template<typename T, typename = std::enable_if_t<std::is_same_v<std::decay_t<T>, value_type>>>
    void push(T&& value)
    {
        data_[end_] = std::forward<T>(value);
        increment();
    }

    // Returns the value at the front of the buffer (the oldest value).
    // This is undefined if the buffer is empty
    const value_type& front() const
    {
        return data_[front_];
    }

    // Removes the value at the front of the buffer (the oldest value)
    void pop()
    {
        if (!empty()) {
            data_[front_].~ValueType();
            decrement();
        }
    }

    // Returns the size of the buffer
    static constexpr std::size_t capacity()
    {
        return Capacity;
    }

    // Returns the number of populated values of the buffer. Its maximum value
    // equals the size of the buffer
    std::size_t size() const
    {
        return size_;
    }

    // Returns whether the buffer is empty
    bool empty() const
    {
        return size_ == 0;
    }

    // Returns whether the buffer is full
    bool full() const
    {
        return size_ == Capacity;
    }

private:

    void increment_or_wrap(std::size_t& value) const
    {
        if (value == Capacity - 1) {
            value = 0;
        } else {
            ++value;
        }
    }

    void increment()
    {
        increment_or_wrap(end_);
        if (full()) {
            increment_or_wrap(front_);
        } else {
            ++size_;
        }
    }

    void decrement()
    {
        increment_or_wrap(front_);
        --size_;
    }

    std::size_t end_{};
    std::size_t front_{};
    std::size_t size_{};
    std::array<value_type, Capacity> data_;
};


} // gem
