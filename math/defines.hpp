#pragma once

#include <stdint.h>
#include <assert.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

#define ER_STATIC_CONSTEXPR static constexpr
#define ER_CONSTEXPR constexpr
#define ER_ASSERT(x) \
    if (!(x)) \
    { \
        printf("Assertion failed: %s\n%s:%d\n", #x, __FILE__, __LINE__); \
        abort(); \
    }

namespace er
{


template<size_t N, size_t K>
constexpr size_t binomial_coefficient()
{
    if constexpr (K > N)
    {
        abort();
        return 0;
    }
    if constexpr (K == 0 || K == N)
    {
        return 1;
    }
    else return binomial_coefficient<N - 1, K - 1>() + binomial_coefficient<N - 1, K>();
}

template<class F, class T>
concept Binop = std::is_convertible_v<std::invoke_result_t<F, T, T>, T>;

namespace detail
{

template<int H, int...N>
struct sequencer_impl: sequencer_impl<H - 1, H - 1, N...>{};

template<int...N>
struct sequencer_impl<0, N...>
{
    ER_STATIC_CONSTEXPR int size = sizeof...(N);
    ER_STATIC_CONSTEXPR int index[sizeof...(N)] = {N...};

    template<template<int...> class F>
    using apply = F<N...>;
    
    template<template<int...> class F>
    using apply_reverse = F<sizeof...(N) - N...>;
};

template<class T>
ER_STATIC_CONSTEXPR T sqrt_newton_rhapson(const T& x, const T& curr, const T& prev)
{
        return curr == prev
            ? curr
            : sqrt_newton_rhapson(x, 0.5 * (curr + x / curr), curr);
}
}

template<int N>
using sequencer = detail::sequencer_impl<N>;

template<int N, template<int...>class F>
using sequenced = typename sequencer<N>::template apply<F>;

template<class T>
ER_STATIC_CONSTEXPR T pi = T(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651);

template<class T>
ER_STATIC_CONSTEXPR T e = T(2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746639193);

template<class T>
ER_STATIC_CONSTEXPR T sqrt(const T& x)
{
    return detail::sqrt_newton_rhapson(x, x, T(0));
}

template<class T>
ER_STATIC_CONSTEXPR T neg(const T& x)
{
	return -x;
}

template<class T>
ER_STATIC_CONSTEXPR T add(const T& x, const T& y)
{
    return x + y;
}

template<class T>
ER_STATIC_CONSTEXPR T sub (const T& x, const T& y)
{
    return x - y;
}

template<class T>
ER_STATIC_CONSTEXPR T mul(const T& x, const T& y)
{
    return x * y;
}

template<class T>
ER_STATIC_CONSTEXPR T div(const T& x, const T& y)
{
    return x / y;
}

template<class T>
ER_STATIC_CONSTEXPR T mod(const T& x, const T& y)
{
    return x % y;
}

template<class T>
ER_STATIC_CONSTEXPR bool less(const T& x, const T& y)
{
    return x < y;
}

template<class T>
ER_STATIC_CONSTEXPR bool greater(const T& x, const T& y)
{
    return x > y;
}

template<class T>
ER_STATIC_CONSTEXPR bool equal(const T& x, const T& y)
{
    return x == y;
}

template<class T>
ER_STATIC_CONSTEXPR bool not_equal(const T& x, const T& y)
{
    return x != y;
}

template<class T>
ER_STATIC_CONSTEXPR bool less_eq(const T& x, const T& y)
{
    return x <= y;
}

template<class T>
ER_STATIC_CONSTEXPR bool greater_eq(const T& x, const T& y)
{
    return x >= y;
}

template<class T>
ER_STATIC_CONSTEXPR T min(const T& x, const T& y)
{
    return x < y ? x : y;
}

template<class T>
ER_STATIC_CONSTEXPR T max(const T& x, const T& y)
{
    return x > y ? x : y;
}

template<class T>
ER_STATIC_CONSTEXPR T abs(const T& x)
{
    return x < 0 ? -x : x;
}

template<int (*F)(int const&, int const&), int H, int...T>
ER_STATIC_CONSTEXPR int fold_v = F(H, fold_v<F, T...>);

template<int (*F)(int const&, int const&), int H>
ER_STATIC_CONSTEXPR int fold_v<F, H> = H;

template<int...N>
ER_STATIC_CONSTEXPR int sum_v = fold_v<add<int>, N...>;

template<int...N>
ER_STATIC_CONSTEXPR int product_v = fold_v<mul<int>, N...>;

template<int...N>
ER_STATIC_CONSTEXPR int min_v = fold_v<min<int>, N...>;

template<int...N>
ER_STATIC_CONSTEXPR int max_v = fold_v<max<int>, N...>;

template<int...N>
ER_STATIC_CONSTEXPR bool has_duplicates = false;

template<int H, int L, int...N>
ER_STATIC_CONSTEXPR bool has_duplicates<H, L, N...> = (H == L) || has_duplicates<H, N...>;

template<int...N>
ER_STATIC_CONSTEXPR bool is_sequence = true;

template<int H, int L, int...N>
ER_STATIC_CONSTEXPR bool is_sequence<H, L, N...> = (H + 1 == L && is_sequence<L, N...>);

}

