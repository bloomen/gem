#pragma once


// A simple circular buffer with a size fixed at compile time.
// ValueType must support default construction. The buffer let's you push
// new values onto the back and pop old values off the front.
template<typename ValueType, int Size>
class circular_buffer {
public:
    using value_type = ValueType;

    static_assert(Size >= 1, "Size must be at least 1");

    // Pushes a new value onto the end of the buffer. If that exceeds the size
    // of the buffer then the oldest value gets dropped (the one at the front).
    void push_back(const value_type& value) {
        data_[index_] = value;
        increment();
    }

    // push_back() overload for r-value references
    void push_back(value_type&& value) {
        data_[index_] = std::move(value);
        increment();
    }

    // Returns the value at the front of the buffer (the oldest value)
    const value_type& front() const {
        return data_[front_];
    }

    // Removes the value at the front of the buffer (the oldest value).
    // Returns true if the front was popped off, false otherwise
    bool pop_front() {
        if (populated_ > 0) {
            data_[front_] = ValueType{};
            decrement();
            return true;
        } else {
            return false;
        }
    }

    // Returns the size of the buffer
    static constexpr int size() {
        return Size;
    }

    // Returns the number of populated values of the buffer. Its maximum value
    // equals the size of the buffer
    int populated() const {
        return populated_;
    }

    // Returns whether the buffer is empty
    bool empty() const {
        return populated_ == 0;
    }

    // Returns whether the buffer is full
    bool full() const {
        return populated_ == Size;
    }

private:

    void increment_or_wrap(int& value) const {
        if (value == Size - 1) {
            value = 0;
        } else {
            ++value;
        }
    }

    void increment() {
        increment_or_wrap(index_);
        if (populated_ == Size) {
            increment_or_wrap(front_);
        } else {
            ++populated_;
        }
    }

    void decrement() {
        increment_or_wrap(index_);
        increment_or_wrap(front_);
        --populated_;
    }

    int index_{};
    int populated_{};
    int front_{};
    value_type data_[Size]{};
};
