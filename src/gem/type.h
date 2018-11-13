#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>


namespace gem {


template<typename Managed>
class type
{
public:

    type(const type&) = default;
    type& operator=(const type&) = default;

    type(type&&) = default;
    type& operator=(type&&) = default;

    template<typename... Args>
    static type<Managed> make(Args&&... args)
    {
        type<Managed> obj;
        obj.managed_ = std::make_shared<Managed>(std::forward<Args>(args)...);
        obj.mutex_ = std::make_shared<std::shared_mutex>();
        return obj;
    }

    template<typename... Args>
    static type<Managed> make_with(std::shared_ptr<std::shared_mutex> mutex, Args&&... args)
    {
        type<Managed> obj;
        obj.managed_ = std::make_shared<Managed>(std::forward<Args>(args)...);
        obj.mutex_ = std::move(mutex);
        return obj;
    }

    static type<Managed> from(std::shared_ptr<Managed> managed)
    {
        type<Managed> obj;
        obj.managed_ = std::move(managed);
        obj.mutex_ = std::make_shared<std::shared_mutex>();
        return obj;
    }

    static type<Managed> from_with(std::shared_ptr<std::shared_mutex> mutex, std::shared_ptr<Managed> managed)
    {
        type<Managed> obj;
        obj.managed_ = std::move(managed);
        obj.mutex_ = std::move(mutex);
        return obj;
    }

    static type<Managed> null()
    {
        type<Managed> obj;
        obj.mutex_ = std::make_shared<std::shared_mutex>();
        return obj;
    }

    static type<Managed> null_with(std::shared_ptr<std::shared_mutex> mutex)
    {
        type<Managed> obj;
        obj.mutex_ = std::move(mutex);
        return obj;
    }

    bool is_null() const
    {
        return managed_ == nullptr;
    }

    bool safe_is_null() const
    {
        std::shared_lock lock{*mutex_};
        return is_null();
    }

    const Managed& get() const
    {
        ensure_not_null();
        return *managed_;
    }

    Managed& get()
    {
        ensure_not_null();
        return *managed_;
    }

    std::shared_mutex& mutex() const
    {
        return *mutex_;
    }

    type<Managed> copy(const bool inherit_mutex=false) const
    {
        type<Managed> obj;
        if (managed_) {
            obj.managed_ = std::make_shared<Managed>(*managed_);
            if (inherit_mutex) {
                obj.mutex_ = mutex_;
            } else {
                obj.mutex_ = std::make_shared<std::shared_mutex>();
            }
        }
        return obj;
    }

    type<Managed> safe_copy(const bool inherit_mutex=false) const
    {
        std::shared_lock lock{*mutex_};
        return copy(inherit_mutex);
    }

    void swap(type<Managed>& obj)
    {
        std::swap(managed_, obj.managed_);
        std::swap(mutex_, obj.mutex_);
    }

    void safe_swap(type<Managed>& obj)
    {
        std::scoped_lock lock{*mutex_, *obj.mutex_};
        swap(obj);
    }

private:
    type() = default;

    void ensure_not_null() const
    {
        if (!managed_) {
            throw std::runtime_error{"type is null"};
        }
    }

    std::shared_ptr<Managed> managed_;
    std::shared_ptr<std::shared_mutex> mutex_;
};


}
