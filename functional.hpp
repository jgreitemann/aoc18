#pragma once
#include <utility>

template <typename F>
struct Y {
    template <typename... X>
    decltype(auto) operator()(X &&... x) const &
    {
        return f(*this, std::forward<X>(x)...);
    }
    F f;
};

template <typename F>
Y(F) -> Y<F>;

template <typename... Bases>
struct overload : Bases... {
    using Bases::operator()...;
};

template <typename... Bases>
overload(Bases...) -> overload<Bases...>;
