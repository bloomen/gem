#include <atomic>


namespace gem {


class spinlock {
public:

    void lock() noexcept {
        while (locked_.test_and_set(std::memory_order_acquire));
    }

    void unlock() noexcept {
        locked_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag locked_ = ATOMIC_FLAG_INIT;
};


} // gem
