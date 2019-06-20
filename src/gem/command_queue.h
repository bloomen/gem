#pragma once

#include <boost/lockfree/queue.hpp>

#include <algorithm>
#include <cassert>
#include <tuple>

namespace gem {

template<std::size_t StorageCapacity=64, std::size_t InitialQueueCapacity=1024>
class command_queue
{
public:

    command_queue()
    {
        assert(queue_.is_lock_free());
    }

    template<typename Object, typename... Args>
    void push(Object* object, void (Object::*functor)(Args...), Args... args)
    {
        command<Object, Args...> cmd{object, functor, std::make_tuple(std::move(args)...)};
        static_assert(sizeof(cmd) <= sizeof(storage), "storage capacity too small");
        storage st;
        auto cmd_begin = reinterpret_cast<unsigned char*>(&cmd);
        std::copy(cmd_begin, cmd_begin + sizeof(cmd), st.data);
        if (size_ == capacity_)
        {
            auto expected = capacity_.load();
            const auto desired = expected * 2;
            while (!capacity_.compare_exchange_weak(expected, desired) && expected != desired);
            queue_.reserve(capacity_);
        }
        if (queue_.push(st))
        {
            ++size_;
        }
    }

    void sync()
    {
        storage st;
        while (queue_.pop(st))
        {
            --size_;
            auto cmd = reinterpret_cast<command_base*>(st.data);
            cmd->execute();
            cmd->~command_base();
        }
    }

private:

    struct storage
    {
        unsigned char data[StorageCapacity];
    };

    struct command_base
    {
        virtual ~command_base() = default;
        virtual void execute() = 0;
    };

    template<int...>
    struct seq
    {};

    template<int N, int... S>
    struct gens : gens<N-1, N-1, S...>
    {};

    template<int... S>
    struct gens<0, S...>
    {
        typedef seq<S...> type;
    };

    template<typename Object, typename... Args, int... S>
    static void call(Object* object, void (Object::*functor)(Args...), const std::tuple<Args...>& args, seq<S...>)
    {
        (object->*functor)(std::get<S>(args)...);
    }

    template<typename Object, typename... Args>
    struct command : command_base
    {
        command(Object* object, void (Object::*functor)(Args...), std::tuple<Args...> args)
        : object(object), functor(functor), args{std::move(args)}
        {}
        void execute() override
        {
            call(object, functor, args, typename gens<static_cast<int>(sizeof...(Args))>::type{});
        }
        Object* object;
        void (Object::*functor)(Args...);
        std::tuple<Args...> args;
    };

    boost::lockfree::queue<storage> queue_{InitialQueueCapacity};
    std::atomic<std::size_t> capacity_{InitialQueueCapacity};
    std::atomic<std::size_t> size_{0};
};

}
