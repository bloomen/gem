#pragma once
#include <forward_list>
#include <memory>
#include "resource.h"


namespace gem {


class resource_pool
{
public:

    resource_pool() = default;

    // delete copy semantics
    resource_pool(const resource_pool&) = delete;
    resource_pool& operator=(const resource_pool&) = delete;

    // default move semantics
    resource_pool(resource_pool&&) = default;
    resource_pool& operator=(resource_pool&&) = default;

    template<typename T, typename... Args, typename Allocator=std::allocator<T>>
    gem::resource<T> allocate(Args&&... args)
    {
        Allocator allocator;
        T* memory = allocator.allocate(1);
        allocator.construct(memory, std::forward<Args>(args)...);
        gem::resource<T> resource{memory, [](T* ptr)
                                            {
                                                Allocator allocator;
                                                allocator.destroy(ptr);
                                                allocator.deallocate(ptr, 1);
                                            }};
        memory_.push_front(resource);
        return resource;
    }

    void garbage_collect()
    {
        memory_.remove_if([](const gem::resource<void>& p)
                          {
                              return p.use_count() == 1;
                          });
    }

private:
    std::forward_list<gem::resource<void>> memory_;
};


} // gem
