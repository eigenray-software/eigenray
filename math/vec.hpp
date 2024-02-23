#pragma once

#include <defines.hpp>
#include <algorithm>
#include <vector>
#include <assert.h>

namespace er
{

template<class T, int N>
struct polynomial;

template<class T, int R, int C>
struct mat;

template<class T, int N>
using col_vec = mat<T, N, 1>;

template<class T, int N>
using row_vec = mat<T, 1, N>;

template<class T, int N>
using col_vec_t = std::conditional_t<1 == N, T, col_vec<T, N>>;

template<class T, int N>
using row_vec_t = std::conditional_t<1 == N, T, row_vec<T, N>>;

template<class T, int R, int C>
using mat_t = std::conditional_t<1 == R && 1 == C, T, mat<T, R, C>>;

template<class T, int R, int C>
struct mat
{
    ER_STATIC_CONSTEXPR bool is_scalar = 1 == R && 1 == C;
    ER_STATIC_CONSTEXPR bool is_vector = !is_scalar && (1 == R || 1 == C);
    ER_STATIC_CONSTEXPR bool is_matrix = !is_scalar && !is_vector;
    ER_STATIC_CONSTEXPR bool is_square = is_matrix && (R == C);
    ER_STATIC_CONSTEXPR int vector_dim = is_vector ? (R + C - 1) : 0;
    ER_STATIC_CONSTEXPR int array_sz = is_matrix ? R : is_vector ? vector_dim : 0;

    using Row_t = row_vec_t<T, C>;
    using Col_t = col_vec_t<T, R>;
    using Data_t = std::conditional_t<1 == C && 1 == R, T, std::conditional_t<1 == R, Col_t[C], Row_t[R]>>;
    using Data_t = std::conditional_t<1 != R, Row_t[R], std::conditional_t<1 != C, Col_t[C], T>>;

    union
    {
        Data_t data;
        T raw_data[R][C];
    };

    mat() : data{} {}

    mat(mat const&) = default;

    static mat identity() requires (is_square)
    {
        mat result;
        for (int i = 0; i < R; ++i)
            result(i, i) = T(1);
        return result;
    }

    mat(T const& arg) requires (is_scalar) : data{ arg } {}
    template<std::convertible_to<T>...Args> requires (is_vector && (sizeof...(Args) == vector_dim))
    mat(Args const&...args) : data{ T(args)...} {}

    
    template<class...Args> requires (is_matrix && (sizeof...(Args) == R*C) && (std::convertible_to<T, Args> && ...))
    mat(Args const&...args) : raw_data{ T(args)... }
    {
    }

    template<class...Args> requires (is_matrix && (sizeof...(Args) == R) && (std::convertible_to<Row_t, Args> && ...))
    mat(Args const&...args) : data{Row_t(args)...} {}

    template<class...Args> requires (is_matrix && (sizeof...(Args) == C) && (std::convertible_to<Col_t, Args> && ...))
    mat(Args const&...args) : mat(transpose(mat<T, C, R>(transpose(args)...))) {}

    operator T const&() const requires (is_scalar) { return data; }
    operator T&() requires (is_scalar) { return data; }

    friend mat<T, C, R> transpose(mat const& m)
    {
        mat<T, C, R> result;
        for (int r = 0; r < R; ++r)
            for (int c = 0; c < C; ++c)
                result(c, r) = m(r, c);
        return result;
	}

    auto& at(int i, int j)
    {
        if constexpr (is_scalar)
        {
            assert(i == 0 && j == 0);
            return data;
        }
        if constexpr (is_vector)
        {
            assert(i == 0 || j == 0);
            return data[i + j];
        }
        if constexpr(is_matrix) 
            return data[i][j];
    }

    auto& operator()(int i, int j) { return at(i, j); }
    auto const& operator()(int i, int j) const { return const_cast<mat*>(this)->at(i, j); }

    auto& operator[](int i) requires( !is_scalar ){ assert(i < array_sz); return data[i]; }
    auto const& operator[](int i) const requires(!is_scalar) { assert(i < array_sz); return data[i]; }

    auto& operator[](int i) requires(is_scalar) { assert(i == 0); return data; }
    auto const& operator[](int i) const requires(is_scalar) { assert(i == 0); return data; }

    template<int M>
    friend mat_t<T, R, M> operator*(mat const& x, mat<T, C, M> const& y)
    {
        mat<T, R, M> result;
        for (int r = 0; r < R; ++r)
            for (int c = 0; c < M; ++c)
                for (int n = 0; n < C; ++n)
                    result(r, c) += x(r, n) * y(n, c);
        return result;
    }

    template<auto F> requires(!std::is_same_v<std::invoke_result_t<decltype(F), T>, void>)
    friend auto transform(mat const& x)
    {
        mat<std::invoke_result_t<decltype(F), T>, R, C> result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                result(i, j) = F(x(i,j));
        return result;
    }

    template<auto F> requires(!std::is_same_v<std::invoke_result_t<decltype(F), T, T>, void>)
    friend auto element_wise(mat const& x, T const& y)
    {
        mat<std::invoke_result_t<decltype(F), T, T>, R, C> result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                result(i, j) = F(x(i, j), y);
        return result;
    }

    template<auto F> requires(!std::is_same_v<std::invoke_result_t<decltype(F), T, T>, void>)
    friend auto element_wise(T const& x, mat const& y)
    {
        mat<std::invoke_result_t<decltype(F), T, T>, R, C> result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                result(i, j) = F(x, y(i, j));
        return result;
    }

    template<auto F> requires(!std::is_same_v<std::invoke_result_t<decltype(F), T, T>, void>)
    friend auto element_wise(mat const& x, mat const& y)
    {
        mat<std::invoke_result_t<decltype(F), T, T>, R, C> result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                result(i, j) = F(x(i, j), y(i, j));
        return result;
    }

    friend mat operator+(mat const& x, mat const& y) { return element_wise<add<T>>(x, y); }
    friend mat operator-(mat const& x, mat const& y) { return element_wise<sub<T>>(x, y); }
    friend mat operator*(mat const& x, T const& y) { return element_wise<mul<T>>(x, y); }
    friend mat operator*(T const& x, mat const& y) { return element_wise<mul<T>>(x, y); }
    friend mat operator/(mat const& x, T const& y) { return element_wise<div<T>>(x, y); }

    friend mat& operator+=(mat& x, mat const& y) { return x = element_wise<add<T>>(x, y); }
    friend mat& operator-=(mat& x, mat const& y) { return x = element_wise<sub<T>>(x, y); }
    friend mat& operator*=(mat& x, T const& y) { return x = element_wise<mul<T>>(x, y); }
    friend mat& operator/=(mat& x, T const& y) { return x = element_wise<div<T>>(x, y); }

    friend mat comp_mul(mat const& x, mat const& y) { return element_wise<mul<T>>(x, y); }
    friend mat comp_div(mat const& x, mat const& y) { return element_wise<div<T>>(x, y); }

    friend row_vec<T, min_v<R, C>> trace(mat const& m)
    {
        row_vec<T, min_v<R, C>> result;
        for (int i = 0; i < min_v<R, C>; ++i)
			result[i] = m(i, i);
        return result;
    }

    friend T fold_mul(mat const& x)
    {
        if constexpr (is_scalar)
        {
			return x;
		}
        else
        {
			T re = 1;
            for (auto& r : x.data)
            {
				if constexpr (is_vector)
					re *= r;
				else re *= fold_mul(r);
			}
			return re;
		}
    }

    friend T fold_add(mat const& x)
    {
        if constexpr (is_scalar)
        {
            return x;
        }
        else
        {
            T re = 0;
            for (auto& r : x.data)
            {
                if constexpr (is_vector)
                    re += r;
                else re += fold_add(r);
            }
            return re;
        }
    }

    mat operator-() const { return transform<neg<T>>(*this); }

    friend std::ostream& operator<<(std::ostream& os, mat const& m)
    {
        if constexpr (1 == R && 1 == C)
			return os << m.data;
        else
        {
            if(is_matrix) os << "[";
            for (int i = 0; i < R; ++i)
            {
				os << '[';
				for (int j = 0; j < C; ++j)
					os << m(i, j) << (j + 1 == C ? "]\n" : ", ");
                if (is_matrix && i != R - 1)
                    os << ',';
			}

            if(is_matrix) os << "]\n";
			return os;
		}
	}

    template<int C2>
    friend mat<T, R, C + C2> operator,(mat const& x, mat<T, R, C2> const& y)
    {
        mat<T, R, C + C2> result;
        for (int i = 0; i < R; ++i)
        {
            for (int j = 0; j < C; ++j)
                result(i, j) = x(i, j);
            for (int j = 0; j < C2; ++j)
                result(i, j + C) = y(i, j);
        }
        return result;
    }

    template<int R2, int C2>
    friend mat<T, R2, C2> slice(mat const& m, int i, int j)
    {
        mat<T, R2, C2> result;
        for (int r = 0; r < R2; ++r)
            for (int c = 0; c < C2; ++c)
                result(r, c) = m((i + r) % R, (j + c) % C);
		return result;
	}

    friend mat<T, R-1,C-1> sub_matrix(mat const& m, int i, int j) requires (is_scalar || is_square)
    {
        mat<T, R-1, C-1> result;
        for (int r = 0; r < R; ++r)
        {
		    if (r == i)
				continue;
            for (int c = 0; c < C; ++c)
            {
				if (c == j)
					continue;
				result(r < i ? r : r - 1, c < j ? c : c - 1) = m(r, c);
			}
		}
		return result;
	}


    friend void LU_decomposition(mat const& m, mat& L, mat& U) requires (is_square)
    {
        // calculate decomposition
        // L is lower triangular
        // U is upper triangular
        // m = L*U

        U = m;
        L = identity();
        for (int i = 0; i < R; ++i)
        {
            for (int j = i + 1; j < R; ++j)
            {
                L(j, i) = U(j, i) / U(i, i);
                U[j] -= U[i] * L(j, i);
            }
        }
	}

    friend T determinant(mat const& m) requires (is_scalar || is_square)
    {
        if constexpr (1 == R)
            return m(0, 0);
        else if constexpr(2 == R)
        {
            return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
        }
        else if constexpr(3 == R)
        {
            return m(0, 0) * m(1, 1) * m(2, 2) + m(0, 1) * m(1, 2) * m(2, 0) + m(0, 2) * m(1, 0) * m(2, 1)
                - m(0, 2) * m(1, 1) * m(2, 0) - m(0, 0) * m(1, 2) * m(2, 1) - m(0, 1) * m(1, 0) * m(2, 2);
        }
        else {
            mat L, U;
            LU_decomposition(m, L, U);
            return fold_mul(comp_mul(trace(L), trace(U)));
        }
    }

    friend mat inverse(mat const& m) requires (is_scalar)
    {
        return T(1) / m;
    }

    friend mat inverse(mat m) requires (is_square)
    {
        mat augmented = identity();

        for (int i = 0; i < R; ++i)
        {
            int pivot = i;
            for (int j = i + 1; j < R; ++j)
                if (std::abs(m(j, i)) > std::abs(m(pivot, i)))
                    pivot = j;
            if (pivot != i)
            {
                std::swap(m[i], m[pivot]);
                std::swap(augmented[i], augmented[pivot]);
            }

            T pivot_value = m(i, i);
            if (pivot_value == T(0))
                return {};

            for (int j = 0; j < C; ++j)
            {
                m(i, j) /= pivot_value;
                augmented(i, j) /= pivot_value;
            }
            for (int j = 0; j < R; ++j)
            {
                if (j == i)
                    continue;
                T factor = m(j, i);
                for (int k = 0; k < C; ++k)
                {
                    m(j, k) -= factor * m(i, k);
                    augmented(j, k) -= factor * augmented(i, k);
                }
            }
        }
        return augmented;
    }

    template<int K>
    friend mat<T, K, K> minor(mat const& m, row_vec<int, K> const& select) requires(is_square)
    {
        mat<T, K, K> result;
        for (int i = 0; i < K; ++i)
            for (int j = 0; j < K; ++j)
                result(i, j) = m(select[i], select[j]);
        return result;
    }

    template<int I, int S = 0> requires (I >= 0 && S >= 0 && S + I <= R)
    static T sum_of_determinants_of_principal_minors_permute(mat const& m, auto& c) requires(is_square)
    {
        if constexpr (I == 0)
        {
            return determinant(minor(m, c));
        }
        else if constexpr (S == R)
        {
            abort();
            return T(0);
        }
        else
        {
            c[I - 1] = S;
            T result = sum_of_determinants_of_principal_minors_permute<I - 1, S + 1>(m, c);
            if constexpr (S + I < R)
                result += sum_of_determinants_of_principal_minors_permute<I, S + 1>(m, c);
            return result;
        }
    }

    template<int K>
    friend T sum_of_determinants_of_principal_minors(mat const& m) requires(is_square)
    {
        if constexpr (R == K)
            return determinant(m);
        else if constexpr (0 == K)
            return T(1);
        else
        {
            row_vec<int, K> select;
            return sum_of_determinants_of_principal_minors_permute<K>(m, select);
        }
    }

    template<int N>
    friend void collect_sum_of_determinants_of_principal_minors(mat const& m, row_vec<T, R + 1>& out) requires(is_square)
    {
        assert(N >= 0 && N <= R);
        out[N] = ((R+N)&1?-1:1)*sum_of_determinants_of_principal_minors<R-N>(m);
        if constexpr (N > 0)
            collect_sum_of_determinants_of_principal_minors<N - 1>(m, out);
	}

    friend polynomial<T, R> characteristic_polynomial(mat const& m) requires(is_square)
    {
		row_vec<T, R+1> result;
        collect_sum_of_determinants_of_principal_minors<R>(m, result);
		return polynomial<T, R>(result);
	}

    template<int N>
    friend mat<T, R, C+N> left_fill(mat const& m) requires(is_matrix)
    {
        return (mat<T, R, N>(), m);
    }

    template<int N>
    friend mat<T, R, C+N> right_fill(mat const& m) requires(is_matrix)
    {
        return (m, mat<T, R, N>());
    }
};


}