#pragma once

#include <boost/lockfree/queue.hpp>

#include <cassert>
#include <cstddef>
#include <tuple>

namespace gem
{

template <std::size_t StorageCapacity = 64, std::size_t QueueCapacity = 1024>
class command_queue
{
public:
    command_queue()
    {
        static_assert(is_power_of_2<StorageCapacity>::value,
                      "StorageCapacity not a power of 2");
        static_assert(is_power_of_2<QueueCapacity>::value,
                      "QueueCapacity not a power of 2");
        assert(queue_.is_lock_free());
    }

    template <typename Object, typename... Args>
    void
    push(Object* object, void (Object::*functor)(Args...), Args&&... args)
    {
        command<Object, Args...> cmd{
            object, functor, std::make_tuple(std::forward<Args>(args)...)};
        static_assert(sizeof(cmd) <= sizeof(storage),
                      "storage capacity too small");
        if (!queue_.push(reinterpret_cast<storage&>(cmd)))
        {
            assert(false && "queue push failed");
        }
    }

    void
    sync()
    {
        storage st;
        while (queue_.pop(st))
        {
            auto& cmd = reinterpret_cast<command_base&>(st);
            cmd.execute();
            cmd.~command_base();
        }
    }

private:
    template <std::size_t Value>
    struct is_power_of_2
    {
        static constexpr auto input = static_cast<int>(Value);
        static constexpr bool value = input && !(input & (input - 1));
    };

    struct storage
    {
        unsigned char data[StorageCapacity];
    };

    struct command_base
    {
        virtual ~command_base() = default;
        virtual void execute() = 0;
    };

    template <typename Object, typename... Args>
    struct command : command_base
    {
        command(Object* object,
                void (Object::*functor)(Args...),
                std::tuple<Args...>&& args)
            : object{object}
            , functor{functor}
            , args{std::move(args)}
        {
        }

        void
        execute() override
        {
            call(std::index_sequence_for<Args...>{});
        }

    private:
        template <std::size_t... Is>
        void call(std::index_sequence<Is...>)
        {
            (object->*functor)(std::get<Is>(args)...);
        }

        Object* object;
        void (Object::*functor)(Args...);
        std::tuple<Args...> args;
    };

    boost::lockfree::queue<storage, boost::lockfree::capacity<QueueCapacity>>
        queue_;
};

} // namespace gem
