#pragma once
#include <memory>


namespace gem {

namespace detail {

template<typename T>
class resource_base
{
public:

    template<typename Deleter>
    resource_base(T* ptr, Deleter deleter)
    : ptr_{ptr, std::move(deleter)}
    {}

    template<typename U>
    resource_base(const resource_base<U>& other)
    : ptr_{other.ptr_}
    {}

    T* get() const noexcept
    {
        return ptr_.get();
    }

    T* operator->() const noexcept
    {
        return ptr_.get();
    }

    long use_count() const noexcept
    {
        return ptr_.use_count();
    }

    std::shared_ptr<T> ptr_;
};

} // detail


template<typename T>
class resource : public detail::resource_base<T>
{
public:

    using detail::resource_base<T>::resource_base;

    T& operator*() const noexcept
    {
        return *this->ptr_;
    }
};


template<>
class resource<void> : public detail::resource_base<void>
{
public:

    using detail::resource_base<void>::resource_base;

};


} // gem
