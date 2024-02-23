#include <complex.hpp>
#include <set>

#undef min
#undef max

namespace er
{

template<class T, int N>
struct polynomial
{
    row_vec<T, 1 + N> data = {};

    polynomial() = default;

    template<class...Args> requires(sizeof...(Args) == (1 + N) && (std::is_convertible_v<Args, T> && ...))
    polynomial(Args&&...args) : data{ T(std::forward<Args>(args))... } {}
    
    friend polynomial operator-(polynomial const& p) { return polynomial(-p.data); }

    template<int M>
    friend polynomial<T, max_v<N,M>> operator- (polynomial const& a, polynomial<T, M> const& b) { return a + -b; }
    template<int M>
    friend polynomial<T, max_v<N,M>> operator+ (polynomial const& a, polynomial<T, M> const& b){ return polynomial<T, max_v<N,M>>(right_fill<max_v<N,M> - N>(a.data) + right_fill<max_v<N,M> - M>(b.data)); }
    
    template<int M>
    friend polynomial<T, N + M> operator* (polynomial const& a, polynomial<T, M> const& b)
    {
        polynomial<T, N + M> result;
        for (int i = 0; i <= N; i++)
            for (int j = 0; j <= M; j++)
                result.data[i + j] += a.data[i] * b.data[j];
        return result;
    }

    friend polynomial<T, N - 1> derivative(polynomial const& p)
    {
        row_vec<T, N> result;
        for (int i = 0; i < N; i++)
            result.data[i] = p.data[i + 1] * (i + 1);
        return result;
    }

    T operator()(T const& x) const
    {
        T result = 0;
        for (int i = N; i >= 0; --i)
            result = result * x + data[i];
        return result;
    }

    friend int leading_coeff_index(polynomial const& p)
    {
        for (int i = N; i >= 0; --i)
        {
            if (abs(p.data[i]) > std::numeric_limits<T>::epsilon())
            {
                assert(i == N);
                return i;
            }
        }
        return -1;
    }

    friend int leading_coeff_sign(polynomial const& p) 
    { 
        return sign(p.data[leading_coeff_index(p)]);
    }

    friend std::pair<T, T> find_bounds(polynomial const& p)
    {
        T r = leading_coeff_sign(p);
        T l = -r;

        while (p(l) * p(r) > 0)
        {
            l *= 2;
            r *= 2;
        }

        return {l, r};
    }

    friend int descartes_rule_of_signs(polynomial const& p)
    {
		int sign_changes = 0;
		int sign = leading_coeff_sign(p);
        for (int i = 0; i < N; i++)
        {
			int new_sign = leading_coeff_sign(derivative(p));
            if (sign != new_sign)
            {
				sign_changes++;
				sign = new_sign;
			}
		}
		return sign_changes;
	}

    friend T find_root_between_bounds(polynomial const& p, T l, T r)
    {
        assert(l < r);
        T pl = p(l);
        if(pl == 0) return l;
        T pr = p(r);
        if(pr == 0) return r;
        const int sl = sign(pl);
        const int sr = sign(pr);
        assert(-1 == sl*sr);

        const bool inc = (sl < sr);
        const bool dec = (sl > sr);

        auto d = derivative(p);
        T x = (l + r) / T(2);
        T grad = d(x);
        T res = p(x);
        
        T midpoint = (l + r) / T(2);

        while (((midpoint != l) && (midpoint != r)) && abs(res) >= std::numeric_limits<T>::epsilon())
        {
            x -= res / grad;
            if (x >= r || x <= l)
                x = (l + r) / T(2);

            res = p(x);
            grad = d(x);

            if (inc && res > 0 && x < r) r = x;
            if (inc && res < 0 && x > l) l = x;

            if (dec && res < 0 && x < r) r = x;
            if (dec && res > 0 && x > l) l = x;

            midpoint = (l + r) / T(2);
        }

        return x;
    }

    friend T find_root_always_increasing_or_decreasing(polynomial const& p)
    {
        if (-1 == leading_coeff_sign(p))
            return find_root_always_increasing_or_decreasing(-p);

        auto [l, r] = find_bounds(p);
        return find_root_between_bounds(p, l, r);
	}

    template<int C>
    friend bool conditional_newton_rhapson(polynomial const& p, int dir, T x, T& root)
    {
        auto d = derivative(p);

        while (abs(d(x)) < 0.001f)
        {
            switch (dir)
            {
            case -1: x -= 1024*std::numeric_limits<T>::epsilon(); break;
            case +1: x += 1024*std::numeric_limits<T>::epsilon(); break;
            }
        }
        
        T init = x;


        T grad = d(x);
        T res = p(x);
        int it = 0;

        while ((it++ < C) && abs(res) >= T(128)*std::numeric_limits<T>::epsilon())
        {
            x -= 0.5 * res / grad;
            res = p(x);
            grad = d(x);

            if (-1 == dir && x > init)
            {
                init -= 1024*std::numeric_limits<T>::epsilon();
                x = init;
                it = 0;
            }
            if (1 == dir && x < init)
            {
                init += 1024 * std::numeric_limits<T>::epsilon();
                x = init;
                it = 0;
            }
        }

        root = x;
    
        return (it < C) && 
            (!std::isnan(res) && !std::isnan(x) && !std::isnan(grad) &&
             !std::isinf(res) && !std::isinf(x) && !std::isinf(grad));
    }


    friend std::vector<T> solve_roots(polynomial const& p)
    {
        std::vector<T> roots;
        if constexpr (N == 1)
        {
            if (p.data[1] != 0)
                roots.push_back(-p.data[0] / p.data[1]);
        }
        else
        {
            auto d = derivative(p);
            auto d_roots = solve_roots(d);
            // if derivative has no roots, then the polynomial is always increasing or decreasing
            // and has at most one root
            if (d_roots.empty())
            {
                roots.push_back(find_root_always_increasing_or_decreasing(p));
			}
            // if derivative has roots, then the polynomial has roots between the roots of the derivative
            // iff polynomial has different signs at two consecutive roots of the derivative
            // or polynomial has a double root at the root of the derivative
            else
            {
                {
                    T r0, r1;
                    bool l = conditional_newton_rhapson<100>(p, -1, d_roots[0], r0);
                    bool r = conditional_newton_rhapson<100>(p, +1, d_roots.back(), r1);

                    if (1 == d_roots.size() && l && r && (r0 - r1) > 0.0000001)
                    {
                        roots.push_back(r0);
                    }
                    else
                    {
                        if (r0 > r1)
                        {
                            std::cout << p << "\n";
                            std::string abc = "123";
                        }
                        if (l)
                            roots.push_back(r0);
                        if (r)
                            roots.push_back(r1);
                    }
                }
                T left = d_roots[0];
				T right;
                for (int i = 1; i < d_roots.size(); i++)
                {
					right = d_roots[i];
					if (sign(p(left)) != sign(p(right)))
						roots.push_back(find_root_between_bounds(p, left, right));
                    else if (p(left) == 0)
                    {
                        roots.push_back(left);
                    }
					left = right;
				}
			}
        }
        
        return roots;
    }
    
    friend std::ostream& operator <<(std::ostream& os, polynomial const& p) 
    { 
        for (int i = N; i >= 0; --i)
        {
            if (p.data[i] != 0)
            {
                if (i < N)
                    os << (p.data[i] > 0 ? " + " : " - ");
                if(i == 0 || abs(abs(p.data[i]) - 1.f) > 0.0001f)
                    os << abs(p.data[i]);
                if (i > 0)
                    os << "x";
                if (i > 1)
                    os << "^" << i;
            }
        }
        
        return os;
    }

    polynomial(row_vec<T, 1 + N> const& data) : data(data) {}
};



}