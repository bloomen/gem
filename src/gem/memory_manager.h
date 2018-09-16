#pragma once
#include <forward_list>
#include <memory>


namespace gem {


class memory_manager
{
public:

    memory_manager() = default;

    // delete copy semantics
    memory_manager(const memory_manager&) = delete;
    memory_manager& operator=(const memory_manager&) = delete;

    // default move semantics
    memory_manager(memory_manager&&) = default;
    memory_manager& operator=(memory_manager&&) = default;

    template<typename T, typename... Args, typename Allocator=std::allocator<T>>
    std::shared_ptr<T> allocate(Args&&... args)
    {
        auto p = std::allocate_shared<T>(Allocator{}, std::forward<Args>(args)...);
        memory_.push_front(p);
        return p;
    }

    void garbage_collect()
    {
        memory_.remove_if([](const std::shared_ptr<void>& p)
                          {
                              return p.use_count() == 1;
                          });
    }

private:
    std::forward_list<std::shared_ptr<void>> memory_;
};


} // gem
