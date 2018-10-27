#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <tuple>


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
        return obj;
    }

    static type<Managed> null()
    {
        return {};
    }

    bool is_null() const
    {
        return managed_ == nullptr;
    }

    bool safe_is_null() const
    {
        std::lock_guard lock{*read_mutex_};
        return managed_ == nullptr;
    }

    type<Managed> deep_copy() const
    {
        type<Managed> obj;
        if (managed_) {
            obj.managed_ = std::make_shared<Managed>(*managed_);
        }
        return obj;
    }

    type<Managed> safe_deep_copy() const
    {
        type<Managed> obj;
        if (managed_) {
            std::lock_guard lock{*read_mutex_};
            obj.managed_ = std::make_shared<Managed>(*managed_);
        }
        return obj;
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

    std::shared_mutex& read_mutex()
    {
        return *read_mutex_;
    }

    std::mutex& write_mutex()
    {
        return *write_mutex_;
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
    std::shared_ptr<std::shared_mutex> read_mutex_ = std::make_shared<std::shared_mutex>();
    std::shared_ptr<std::mutex> write_mutex_ = std::make_shared<std::mutex>();
};


}
