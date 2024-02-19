#pragma once
#include <defines.hpp>

namespace er
{

template<class T>
struct Complex
{
    T r, i;

    Complex() : r{}, i{} {}

    Complex(T const& r) : r(r), i{} {}
    Complex(T const& r, T const& i) : r(r), i(i) {}
    Complex(Complex const& c) = default;

    Complex& operator=(Complex const& c) = default;

    T magsq() const { return r * r + i * i; }
    Complex operator~() const { return {r, -i}; }
    friend Complex operator+(Complex const& l, Complex const& r) { return {l.r + r.r, l.i + r.i}; }
    friend Complex operator-(Complex const& l, Complex const& r) { return {l.r - r.r, l.i - r.i}; }
    friend Complex operator*(Complex const& l, Complex const& r) { return {l.r * r.r - l.i * r.i, l.r * r.i + l.i * r.r}; }
    friend Complex operator/(Complex const& l, Complex const& r) { return l * ~r / r.magsq(); }
    friend Complex operator*(Complex const& l, T const& r) { return {l.r * r, l.i * r}; }
    friend Complex operator*(T const& l, Complex const& r) { return {l * r.r, l * r.i}; }
    friend Complex operator/(Complex const& l, T const& r) { return {l.r / r, l.i / r}; }
    friend Complex operator/(T const& l, Complex const& r) { return l * ~r / r.magsq(); }
    friend Complex& operator+=(Complex& l, Complex const& c) { return l = l + c; }
    friend Complex& operator-=(Complex& l, Complex const& c) { return l = l - c; }
    friend Complex& operator*=(Complex& l, Complex const& c) { return l = l * c; }
    friend Complex& operator/=(Complex& l, Complex const& c) { return l = l / c; }
    friend Complex& operator*=(Complex& l, T const& r) { return l = l * r; }
    friend Complex& operator/=(Complex& l, T const& r) { return l = l / r; }
    friend bool operator==(Complex const& l, Complex const& r) { return l.r == r.r && l.i == r.i; }
    friend bool operator!=(Complex const& l, Complex const& r) { return !(l == r); }
    friend Complex exp(Complex const& c) { return std::exp(c.r) * Complex(std::cos(c.i), std::sin(c.i)); }

    friend std::ostream& operator<<(std::ostream& os, Complex const& c) { return os << c.r << " + " << c.i << "i"; }
};

template<class T>
struct Quat
{
    T r, i, j, k;

    Quat() : r{}, i{}, j{}, k{} {}
    Quat(T const& r, T const& i, T const& j, T const& k) : r(r), i(i), j(j), k(k) {}
    Quat(Quat const& q) = default;
    Quat& operator=(Quat const& q) = default;
    T magsq() const { return r * r + i * i + j * j + k * k; }

    Quat operator~() const { return {r, -i, -j, -k}; }
    friend Quat operator+(Quat const& l, Quat const& r) { return {l.r + r.r, l.i + r.i, l.j + r.j, l.k + r.k}; }
    friend Quat operator-(Quat const& l, Quat const& r) { return {l.r - r.r, l.i - r.i, l.j - r.j, l.k - r.k}; }
    friend Quat operator*(Quat const& l, Quat const& r) 
    { 
        return {
            l.r * r.r - l.i * r.i - l.j * r.j - l.k * r.k,
            l.r * r.i + l.i * r.r + l.j * r.k - l.k * r.j,
            l.r * r.j - l.i * r.k + l.j * r.r + l.k * r.i,
            l.r * r.k + l.i * r.j - l.j * r.i + l.k * r.r
        };
    }
    friend Quat operator/(Quat const& l, Quat const& r) { return l * ~r / r.magsq(); }
    friend Quat operator*(Quat const& l, T const& r) { return {l.r * r, l.i * r, l.j * r, l.k * r}; }
    friend Quat operator*(T const& l, Quat const& r) { return {l * r.r, l * r.i, l * r.j, l * r.k}; }
    friend Quat operator/(Quat const& l, T const& r) { return {l.r / r, l.i / r, l.j / r, l.k / r}; }
    friend Quat operator/(T const& l, Quat const& r) { return l * ~r / r.magsq(); }
    friend Quat& operator+=(Quat& l, Quat const& q) { return l = l + q; }
    friend Quat& operator-=(Quat& l, Quat const& q) { return l = l - q; }
    friend Quat& operator*=(Quat& l, Quat const& q) { return l = l * q; }
    friend Quat& operator/=(Quat& l, Quat const& q) { return l = l / q; }
    friend Quat& operator*=(Quat& l, T const& r) { return l = l * r; }
    friend Quat& operator/=(Quat& l, T const& r) { return l = l / r; }
    friend bool operator==(Quat const& l, Quat const& r) { return l.r == r.r && l.i == r.i && l.j == r.j && l.k == r.k; }
    friend bool operator!=(Quat const& l, Quat const& r) { return !(l == r); }
    friend Quat exp(Quat const& q) 
    { 
        T theta = std::sqrt(q.i * q.i + q.j * q.j + q.k * q.k);
        T s = std::sin(theta);
        T c = std::cos(theta);
        return {c, s * q.i / theta, s * q.j / theta, s * q.k / theta};
    }

    friend std::ostream& operator<<(std::ostream& os, Quat const& q) { return os << q.r << " + " << q.i << "i + " << q.j << "j + " << q.k << "k"; }
};

template<class T, int N>
row_vec<T, N> derivative(row_vec<T, N> const& poly)
{
    row_vec<T, N > result = {};
    // sorted from highest to lowest power
    // implicit 1 for x^N which is not present in the array
    for (int i = 1; i < N; i++)
		result[i] = poly[i-1] * (N - i);
    return result;
}

template<class T, int N>
Complex<T> eval_polynomial(row_vec<Complex<T>, N> const& poly, Complex<T> x)
{
    Complex<T> result = 0;
    for (int i = 0; i < N; i++)
        result = result * x + poly[i];
    return result;
}

template<class T, int N>
row_vec<T, N> divide_polynomial_by_root(row_vec<T, N> const& poly, T const& x)
{
    row_vec<T, N - 1> result = {};
	result[N - 2] = poly[N - 1];
	for (int i = N - 3; i >= 0; i--)
		result[i] = poly[i + 1] + x * result[i + 1];
	return (mat<T,1,1>(), result);
}

template<class T, int N>
std::vector<Complex<T>> solve_polynomial(row_vec<T, N> const& poly)
{
    auto tmp = (mat<T, 1, 1>(1), poly);
    auto p = transform<[](auto const& c) { return Complex<T>(c); }>(tmp);
    auto d = derivative<Complex<T>, N + 1>(p);
    
    Complex<T> guess = 0;
    Complex<T> res  = eval_polynomial<T, N + 1>(p, guess);
    Complex<T> grad = eval_polynomial<T, N + 1>(d, guess);

    std::vector<Complex<T>> roots;

    while(roots.size() < N)
    {
		guess -= res / grad;
        res  = eval_polynomial<T, N + 1>(p, guess);
        grad = eval_polynomial<T, N + 1>(d, guess);
        if (res.magsq() < 0.0001)
        {
            roots.push_back(guess);
            p = divide_polynomial_by_root<Complex<T>, N+1>(p, guess);
            d = derivative<Complex<T>, N + 1>(p);
            guess = 0;
        }
	}

    return roots;
}

}