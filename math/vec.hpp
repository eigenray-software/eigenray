#pragma once

#include <defines.hpp>
#include <algorithm>

namespace er
{

template<class T, int N>
struct vec;

template<class T>
struct collapse_vec { using type = T; };

template<class T> 
struct collapse_vec<vec<T, 1>> : collapse_vec<T> {};

template<class T, int N> 
struct collapse_vec<vec<T, N>>
{
    using type = vec<typename collapse_vec<T>::type, N>;
};

template<class T>
using collapse_vec_t = collapse_vec<T>::type;

template<class T> 
struct vec_dim : std::integral_constant<int, 1> { using type = T; };

template<class T, int N> 
struct vec_dim<vec<T,N>> : std::integral_constant<int, N> { using type = T; };

template<class T> 
ER_STATIC_CONSTEXPR int vec_dim_v = vec_dim<T>::value;

template<class T> 
using vec_dim_t = typename vec_dim<T>::type;

template<class T>
ER_STATIC_CONSTEXPR bool is_vec = false;

template<class T, int N>
ER_STATIC_CONSTEXPR bool is_vec<vec<T, N>> = true;

template<class T, int N>
struct vec 
{
    T data[N];

    vec() : data{} {}

    explicit vec(T const& x) 
    {
        std::fill_n(data, N, x);
    }

    vec(vec const& v) = default;
    
    template<std::convertible_to<T>...Args> requires (sizeof...(Args) == N)
    vec(Args&&...args) : data{T(args)...}
    {
    }

    using collapsed = collapse_vec_t<vec>;
    
    operator collapsed() const
    {
        return std::bit_cast<collapsed, vec>(*this);
    }

    operator vec<collapsed, 1>() const requires(std::is_same_v<collapsed, vec>)
    {
        return std::bit_cast<vec<vec, 1>, vec>(*this);
    }

    operator vec<vec<collapse_vec_t<T>, 1>, N>() const requires(std::is_same_v<collapsed, vec>)
    {
        return std::bit_cast<vec<vec<T, 1>, N>, vec>(*this);
    }

    template<auto F> requires(!std::is_same_v<std::invoke_result_t<decltype(F), T, T>, void>)
    static auto element_wise(vec const& x, vec const& y)
    {
        vec<std::invoke_result_t<decltype(F), T, T>, N> result;
        for (int i = 0; i < N; ++i)
            result.data[i] = F(x.data[i], y.data[i]);
        return result;
    }

    friend vec operator+(vec const& x, vec const& y) { return element_wise<add<T>>(x, y); }
    friend vec& operator+=(vec& x, vec const& y) { return x = element_wise<add<T>>(x, y); }
    friend vec operator-(vec const& x, vec const& y) { return element_wise<sub<T>>(x, y); }
    friend vec& operator-=(vec& x, vec const& y) { return x = element_wise<sub<T>>(x, y); }
    friend vec operator*(vec const& x, vec const& y) { return element_wise<mul<T>>(x, y); }
    friend vec& operator*=(vec& x, vec const& y) { return x = element_wise<mul<T>>(x, y); }
    friend vec operator/(vec const& x, vec const& y) { return element_wise<div<T>>(x, y); }
    friend vec& operator/=(vec& x, vec const& y) { return x = element_wise<div<T>>(x, y); }
    friend vec operator%(vec const& x, vec const& y) { return element_wise<mod<T>>(x, y); }
    friend auto operator<(vec const& x, vec const& y) { return element_wise<less<T>>(x, y); }
    friend auto operator <=(vec const& x, vec const& y) { return element_wise<less_eq<T>>(x, y); }
    friend auto operator>(vec const& x, vec const& y) { return element_wise<greater<T>>(x, y); }
    friend auto operator>=(vec const& x, vec const& y) { return element_wise<greater_eq<T>>(x, y); }
    friend auto operator==(vec const& x, vec const& y) { return element_wise<equal<T>>(x, y); }
    friend auto operator!=(vec const& x, vec const& y) { return element_wise<not_equal<T>>(x, y); }

    friend vec<vec, 2> operator, (vec const& x, vec const& y) { return {x, y}; }

    template<int M>
    friend vec<vec, M+1> operator, (vec<vec, M> const& x, vec const& y) 
    { 
        vec<vec, M+1> result;
        std::copy_n(x.data, M, result.data);
        result.data[M] = y;
        return result;
    }

    template<class U> requires (!std::is_convertible_v<U, T>)
    friend void operator, (vec x, U y) = delete;

    template<class U> requires (!std::is_convertible_v<U, T>)
    friend void operator, (U, vec) = delete;

    template<int M>
    friend vec<vec, M+1> operator, (vec const& x, vec<vec, M> const& y) 
    { 
        vec<vec, M+1> result;
        result.data[0] = x;
        std::copy_n(y.data, M, result.data + 1);
        return result; 
    }

    friend std::ostream& operator<<(std::ostream& os, vec const& v)
    {
        os << '[' << v.data[0];
        for (int i = 1; i < N; ++i)
            os << ", " << v.data[i];
        return os << ']';
    }

    T const& operator[](int i) const { return data[i]; }
    T& operator[](int i) { return data[i]; }
};

template <class T, int R, int N, int C>
vec<vec<T, C>, R> operator*(vec<vec<T, N>, R> const &x, vec<vec<T, C>, N> const &y)
{
    vec<vec<T, C>, R> result;
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c)
            for (int n = 0; n < N; ++n)
                result[r][c] += x[r][n] * y[n][c];
    return result;
}

template <class T, int N, int C> requires (N > 1)
vec<T, C> operator*(vec<T, N> const &x, vec<vec<T, C>, N> const &y)
{
    return vec<vec<T, N>, 1>(x) * y;
}

template <class T, int R, int N> requires (N > 1)
vec<T, R> operator*(vec<vec<T, N>, R> const &x, vec<T, N> const &y)
{
    return x * vec<vec<T, 1>, N>(y);
}

}