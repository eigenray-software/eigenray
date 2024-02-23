#pragma once
#include <vec.hpp>

namespace er
{

template<class F, int N = 0>
struct complex;

template<class T>
ER_STATIC_CONSTEXPR bool is_complex = false;

template<class T, int N>
ER_STATIC_CONSTEXPR bool is_complex<complex<T, N>> = true;

template<class T>
T conjugate(T const& c) 
{ 
    if constexpr (is_complex<T>) 
        return ~c; 
    else 
        return c;
}

template<class F, int N>
struct complex
{
    using data_t = std::conditional_t<0 == N, F, complex<F, N - 1>>;

    union
    {
        struct
        {
            data_t x;
            data_t y;
        };
        row_vec<data_t, 2> data;
        F raw_data[2<<N];
        row_vec<F, (2 << N)> raw_vec;
    };

    complex(F const& scalar = F(0)) : raw_data{scalar} {}    
    
    template<class...Args> 
        requires(sizeof...(Args) == (2<<N) && (std::is_convertible_v<Args, F> && ...))
    complex(Args&&...args) : raw_data{ F(std::forward<Args>(args))... } {}

    complex(data_t const& x, data_t const& y) : data(x, y) {}

    friend F magsq(complex const& c) { return fold_add(comp_mul(c.raw_vec, c.raw_vec)); }
    friend complex unit(complex const& c) { return c / sqrt(magsq(c)); }
    friend complex inverse(complex const& c) { return ~c / magsq(c); }
    friend complex operator -(complex const& c) { return {-c.x, -c.y}; }
    friend complex operator ~(complex const& c) { return {conjugate(c.x), -c.y}; }
    friend complex operator +(complex const& a, complex const& b) { return {a.data + b.data}; }
    friend complex operator -(complex const& a, complex const& b) { return {a.data - b.data}; }

    friend complex operator *(complex const& a, complex const& b) { return complex(a.x * b.x - conjugate(b.y) * a.y, b.y * a.x + a.y * conjugate(b.x)); }
    friend complex operator /(complex const& a, complex const& b) { return a * inverse(b); }
    friend complex operator *(F const& a, complex const& b) { return {a * b.x, a * b.y}; }
    friend complex operator *(complex const& a, F const& b) { return {a.x * b, a.y * b}; }
    friend complex operator /(complex const& a, F const& b) { return {a.x / b, a.y / b}; }
    friend complex operator /(F const& a, complex const& b) { return a * inverse(b); }

    friend bool operator ==(complex const& a, complex const& b) { return a.data == b.data; }
    friend bool operator !=(complex const& a, complex const& b) { return a.data != b.data; }

    friend std::ostream& operator <<(std::ostream& os, complex const& c) 
    { 
        os << "(" << c.raw_data[0] << ", ";
        for (int i = 1; i < (2 << N); ++i)
        {
			os << c.raw_data[i];
            if (i < ((2<<N) - 1))
				os << ", ";
		}
        return os << ")";
    }

private:
    complex(row_vec<data_t, 2> const& data) : data(data) {}
};

template<class T>
using quaternion = complex<T, 1>;

template<class T>
using octonion = complex<T, 2>;

template<class T>
using sedonion = complex<T, 3>;

}