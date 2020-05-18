#pragma once

#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

namespace gem
{

class Error
{
public:
    static constexpr int CODE_UNSET = std::numeric_limits<int>::max();

    explicit Error() = default;
    explicit Error(std::string message)
        : m_message{std::move(message)}
    {}
    explicit Error(const int code)
        : m_code{code}
    {}
    explicit Error(std::string message, const int code)
        : m_message{std::move(message)}, m_code{code}
    {}

    const std::string& message() const
    {
        return m_message;
    }

    int code() const
    {
        return m_code;
    }

    std::string repr() const
    {
        std::string str = "Error: ";
        str += m_message;
        if (m_code != CODE_UNSET)
        {
            str += " (" + std::to_string(m_code) + ")";
        }
        return str;
    }

private:
    std::string m_message;
    int m_code = CODE_UNSET;
};

class ResultError : public std::runtime_error
{
public:
    explicit ResultError(const std::string& message)
        : std::runtime_error{message}
    {}
};

template<typename Data>
class Result
{
public:
    static_assert(!std::is_same_v<Data, Error>, "Data cannot be Error");

    Result(const Data& data)
        : m_result{data}
    {}
    Result(Data&& data)
        : m_result{std::move(data)}
    {}
    Result(Error error)
        : m_result{std::move(error)}
    {}

    bool ok() const
    {
        return std::holds_alternative<Data>(m_result);
    }

    const Error& error() const
    {
        return std::get<Error>(m_result);
    }

    const Data& data() const
    {
        return std::get<Data>(m_result);
    }

    template<typename DataFunctor, typename ErrorFunctor>
    void match(DataFunctor&& data_functor, ErrorFunctor&& error_functor) const &
    {
        if (ok())
        {
            std::forward<DataFunctor>(data_functor)(data());
        }
        else
        {
            std::forward<ErrorFunctor>(error_functor)(error());
        }
    }

    template<typename DataFunctor, typename ErrorFunctor>
    void match(DataFunctor&& data_functor, ErrorFunctor&& error_functor) &&
    {
        if (ok())
        {
            std::forward<DataFunctor>(data_functor)(std::get<Data>(std::move(m_result)));
        }
        else
        {
            std::forward<ErrorFunctor>(error_functor)(error());
        }
    }

    const Data& unwrap() const &
    {
        if (!ok())
        {
            throw ResultError{error().repr()};
        }
        return data();
    }

    Data&& unwrap() &&
    {
        if (!ok())
        {
            throw ResultError{error().repr()};
        }
        return std::get<Data>(std::move(m_result));
    }

private:
    std::variant<Data, Error> m_result;
};

}
