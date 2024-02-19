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


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-1, 1);

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
            m(i, j) = 5 * dis(gen);
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

int main()
{
    srand(time(0));

    // std::cout << binomial_coefficient<10, 5>() << "\n";
    // std::cout << binomial_coefficient<11, 7>() << "\n";
    // return 1;
    while (1)
    {
        auto m = random_invertbile_matrix<4>();
        // std::cout << m << "\n";

  
        // auto m = random_invertbile_matrix<3>();
        
        auto poly = characteristic_polynomial(m);
        std::cout << poly;
        std::cout << derivative<f32, 4>(poly);

        for (auto& root : solve_polynomial<f32, 4>(poly))
        {
            std::cout << "\t\t" << root << "\n";
        }
        std::cout << "-------------------\n";
        // std::cout << determinants_of_principal_minors<5>(m) << "\n";
        // std::cout << eigenvalues(m);
        continue;
        //std::cout << inverse(m);
        //std::cout << "---------------\n";
        // std::cout << 
        // std::cout << m*
            
        // auto err = m*iterative_convergent_inverse(m) - m.identity();
        // std::cout << "\t\tError:" << fold_add(transform<[](auto const& c) { return c * c; }>(err)) << "\n";
    }

    //{
    //    // create a random invertible matrix

    //    auto m = random_invertbile_matrix<3>();
    //    std::cout << m;
    //    std::cout << "\n\n";
    //    std::cout << iterative_convergent_inverse(iterative_convergent_inverse(m));
    //    std::cout << "\n\n";
    //    std::cout << iterative_convergent_inverse(m);
    //    std::cout << "\n\n";
    //    std::cout << (m * iterative_convergent_inverse(m));
    //    std::cout << "\n\n";
    //}

    return 1;

    row_vec<f32, 4> v = { 1.f, 2.f, 3.f, 4.f };
    col_vec<f32, 4> u = { 1.f, 2.f, 3.f, 4.f };
    row_vec<f32, 5> u2 = { 1.f, 2.f, 3.f, 4.f, 5.f };

    /*   mat<f32, 4, 4> m0 = { v, v, v, v };*/
    mat<f32, 4, 5> m1 = { u, u, u, u, u };
    mat<f32, 4, 5> m2 = { u2, u2, u2, u2 };

    std::cout << (u) << "\n";
    std::cout << (u2) << "\n\n";
    std::cout << (((u, u, u))) << "\n";
    std::cout << (((u, u))) << "\n";
    std::cout << (((u, u, u), (u, u))) << "\n";

    auto w0 = v * u;
    auto w1 = u * v;
    std::cout << typeid(v[0]).name() << "\n";
    std::cout << typeid(u[0]).name() << "\n";
    std::cout << typeid(v(0, 0)).name() << "\n";
    std::cout << typeid(u(0, 0)).name() << "\n";
    std::cout << "\n";
    std::cout << typeid(v * u).name() << "\n";
    std::cout << typeid(u * v).name() << "\n";
    std::cout << "\n";
    std::cout << typeid(row_vec<f32, 4>).name() << "\n";
    std::cout << typeid(row_vec<f32, 4>::Row_t).name() << "\n";
    std::cout << typeid(row_vec<f32, 4>::Col_t).name() << "\n";
    std::cout << typeid(row_vec<f32, 4>::data).name() << "\n";
    std::cout << "\n";
    std::cout << typeid(col_vec<f32, 4>).name() << "\n";
    std::cout << typeid(col_vec<f32, 4>::Row_t).name() << "\n";
    std::cout << typeid(col_vec<f32, 4>::Col_t).name() << "\n";
    std::cout << typeid(col_vec<f32, 4>::data).name() << "\n";
    std::cout << "\n";
    std::cout << typeid(mat<f32, 4, 4>).name() << "\n";
    std::cout << typeid(mat<f32, 4, 4>::Row_t).name() << "\n";
    std::cout << typeid(mat<f32, 4, 4>::Col_t).name() << "\n";
    std::cout << typeid(mat<f32, 4, 4>::data).name() << "\n";
    //std::cout << ((v, v, v, v)) << "\n";
    //std::cout << ((v * (v, v, v, v))) << "\n";
    //std::cout << (((v, v, v, v) * v)) << "\n";

    //Instance instance;
    //auto vk = instance.devices[0];

    //ShaderModule shader(vk, read_binary("a.spv"));

    //Buffer buf(vk, 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    //
    //return 0;
}


