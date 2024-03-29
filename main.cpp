#include <vec.hpp>
#include <vk.hpp>

#include <iostream>
#include <Windows.h>

#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iterator>

#include <random>

#include <set.hpp>
#include <complex.hpp>
#include <polynomial.hpp>
#include <iomanip>
#include <rational.hpp>
#include <num.hpp>


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-1, 1);
std::uniform_real_distribution<> dis12(1, 2);
std::uniform_real_distribution<> dis01(0, 1);
std::uniform_int_distribution<i64> dis_int(INT_MIN, INT_MAX);

using namespace er;

std::vector<u8> read_binary(std::string const& file)
{
    std::ifstream f(file, std::ios::binary);
    std::istreambuf_iterator start(f);
    return std::vector<u8>(start, decltype(start){});
}

template<int N>
mat<f32, N, N> random_invertbile_matrix()
{
    mat<f32, N, N>  m;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            m(i, j) =  dis(gen);
        }
	}

    auto det = determinant(m);
    // check if the matrix is invertible
    // if not, generate a new one
    if (abs(det) < 0.1)
    {
        return random_invertbile_matrix<N>();
	}
    return m;
}

template<int N>
std::pair<f32, f32> normalize_determinant(f32 target_det, mat<f32, N, N>& m)
{
    f32 initial_det = determinant(m);
	f32 scaling = pow(target_det / abs(initial_det), 1.f / N);
	m *= scaling;
    return { scaling, initial_det };
}

template<int N>
mat<f32, N, N> iterative_convergent_inverse(mat<f32, N, N> m)
{
    const f32 target_det = 1.f;
    auto [scaling, initial_det] = normalize_determinant(target_det, m);

    mat<f32, N, N> inv = transpose(m);
    mat<f32, N, N> delta = inv * m - m.identity();
    auto cost = fold_add(transform<[](auto const& c) { return c * c; }>(delta));

    //  1x4 * 4x1 = 1

    f32 lr = 0.05f;

    u32 it = 0;
    u32 inner_it = 0;

    struct CostHistory
    {
		f32 cost;
		mat<f32, N, N> inv;

        bool operator < (CostHistory const& c) const
        {
			return cost < c.cost;
        }
	};

    std::set<CostHistory> outer_history;
    std::set<CostHistory> history;
    history.insert({ cost, inv });
    while (cost > 0.0001F)
    {
        if (history.rbegin()->cost > cost)
        {
			history.insert({ cost, inv });
            if (history.size() > 100)
            {
				history.erase(*history.rbegin());
            }
		}

        ++it;
        ++inner_it;
        auto gradient = delta * transpose(m);
        if (inner_it > 77776)
        {
            inv = history.begin()->inv;
            break;
            auto what = determinant(gradient);
            if (abs(what) < 0.0001)
            {
                outer_history.insert(*history.begin());
                history.clear();

                // we hit a local minimum
                // randomize guess and try again
                inv += random_invertbile_matrix<N>();
                delta = inv * m - m.identity();
                cost = fold_add(transform<[](auto const& c) { return c * c; }>(delta));
                inner_it = 0;
                history.insert({ cost, inv });
                continue;
            }
            std::string abc = "123";
            // wtf?
        }
        auto costsq = sqrt(cost);

        inv -= lr * gradient;
  //      for (int i = 0; i < N; i++)
  //      {
  //          for (int j = 0; j < N; j++)
  //          {
  //              f32 what = delta[i] * transpose(m[j]);
  //              inv(i, j) -= (lr / costsq) * what;
		//	}
		//}

        delta = (inv * m - m.identity());
        cost = fold_add(transform<[](auto const& c) { return c * c; }> (delta));
	}
    
   
    if (fold_add(transform < [](auto const& c) { return std::isnan(c);  } > (inv)))
    {
        std::cout << "[" << (initial_det < 0 ? '-' : '+') << " " << scaling << " | " << initial_det << "]\n";
        // std::cout << "Iterations: " << it << "\n";
        // std::cout << "determinant: " << initial_det << " " << new_det << "\n";
    }
    else
    {
        std::cout << "\t\tIterations: " << it << " || " << (initial_det < 0 ? '-' : '+') << " " << scaling << " | " << initial_det << "\n";
    }

    return scaling * inv;
}

template<class T, int R, int C>
polynomial<T, R + 2 * C> generate_poly(
    std::vector<T>& out_real_roots, 
    std::vector<complex<T>>& out_complex_roots)
{
    if constexpr (R == 0 && C == 0)
    {
        return polynomial<T, 0>{1};
	}
	else if constexpr (R == 0)
	{
		f32 a = dis(gen);
        f32 b = (dis(gen) * 0.25f + 0.5f);
		auto p = polynomial<f32, 2>{ a * a + b * b, -2 * a, 1 };
        out_complex_roots.push_back({a, +b});
        out_complex_roots.push_back({a, -b});
		return p * generate_poly<T, R, C - 1>(out_real_roots, out_complex_roots);
	}
	else if constexpr (C == 0)
	{
        T root = sign(dis(gen)) * dis12(gen) * 0.5;
        out_real_roots.push_back(root);
		return polynomial<T, 1>{-root, 1} * generate_poly<T, R - 1, C>(out_real_roots, out_complex_roots);
    }
    else
    {
        return generate_poly<T, R, 0>(out_real_roots, out_complex_roots) * generate_poly<T, 0, C>(out_real_roots, out_complex_roots);
    }
}

#include <immintrin.h>

int main()
{
    srand(time(0));
    auto a = num(-1);
    auto b = -a;
    std::cout << b << "\n";
    std::cout << gcd(num(2*3*5*7*11* 57386), num(2 * 3 * 5 * 7 * 11 * 57387)) << "\n";

    std::cout << rational(sqrt(2.f)) << "\n";
    std::cout << (sqrt(2.f)) << "\n";

    return 1;
    //num a = 0xff * 0xff * 0xff;
    //num b = a + a;
    //num c = -a;

    //std::hex(std::cout);
    //std::cout << (a+-b) << "\n";
    //std::cout << (2*(0xff * 0xff * 0xff) - (0xff * 0xff * 0xff)) << "\n";
    //std::cout << num(((0xff * 0xff * 0xff) - 2*(0xff * 0xff * 0xff))) << "\n";
    size_t i = 0;

    //std::cout << -0b11011001010000100110100011111111ull << "\n";
    //return 1;

    auto opc = "%";

    auto op = [](auto const& a, auto const& b)
    {
        return a % b;
    };

    while (1)
    {
        i64 a = dis_int(gen);
        i64 b = dis_int(gen);
        i64 c = op(a,b);
        num x = a;
        num y = b;
        num z = op(x, y);
        if(z != c)
		{
            std::cout << a << " " << opc << " " << b << " = " << c << "\n";
            std::hex(std::cout);
            std::cout << c << "\n";
            std::cout << u64(z.extension) << "\n";
            std::cout << u64(num(c).extension) << "\n";
			std::cout << "Error: " << x << " " << opc << " " << y << " = " << (z) << " != " << num(c) << "\n";
			break;
		}

        if(++i % 10000 == 0)
		{
			std::cout << num(i) << "\n";
		}
    }
    // std::cout << binomial_coefficient<10, 5>() << "\n";
    // std::cout << binomial_coefficient<11, 7>() << "\n";
    // return 1;

    //complex<f32> c{1, 1};
    //c = unit(c);
    //quaternion<f32> q{c, {}};
    //q = unit(q);
    //octonion<f32> o{q, {}};
    //o = unit(o);
    //std::cout << (o * o) << "\n";

    f64 err_sum    = 0;
    f32 max_error  = 0;
    size_t counter = 0;
    size_t root_counter = 0;


    return 1;
    while (1)
    {
        //f32 a = dis(gen);
        //f32 b = (dis(gen) * 0.25f + 0.5f) * 1024.f;
        //f32 c = dis(gen);
        //auto p = polynomial<f32, 2>{ a * a + b * b, -2 * a, 1 } *polynomial<f32, 1>{-c, 1};
        //if (p(c) != 0.f)
        //    continue;

        static constexpr int R = 5;
        std::vector<f32> gen_real_roots;
        std::vector<complex<f32>> gen_complex_roots;
        auto p = generate_poly<f32, R, 1>(gen_real_roots, gen_complex_roots);

        for (auto& r : gen_real_roots)
        {
            if(p(r) != 0.f)
			{
				continue;
			}
        }
        //auto m = random_invertbile_matrix<5>();
        //auto p = characteristic_polynomial(m);

        ++counter;
        const int r = 1000000;
        if (0 == counter % r)
        {
            std::cout << "Still counting " << (counter/r) << "\n";
        }

        std::sort(gen_real_roots.begin(), gen_real_roots.end());
        auto roots = solve_roots(p);
        
        if (roots.size() != R)
        {

            std::string abc = "!23";
        }
        continue;
        if (!roots.empty())
        {
            std::cout << p << "\n";
            for (auto root : roots)
            {
                f32 err = abs(p(root)) / std::numeric_limits<f32>::epsilon();
                std::cout << "#\t\t" << root << " : " << p(root) << "\n";
                err_sum += err;
                ++root_counter;
                if (err > max_error)
                {
                    max_error = err;
                    std::cout << p << "\n";
                    std::cout << root << "\n";
                    std::cout << "Max error: " << max_error << "\n";
                    std::cout << "Average error: " << (err_sum / root_counter) << "\n";
                }
            }
        }
    }

}


