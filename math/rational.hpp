#include <num.hpp>

namespace er
{
struct rational
{
    num  nom, denom;

    rational() = default;

    rational(f32 f)
    {
        ER_CONSTEXPR u32 mantissa_mask = (1<<23)-1;
        int exp = 0;
        f = std::frexp(f, &exp);
        u32 digits = ((u32&)f & mantissa_mask) | (1 << 23);
        nom = num(digits);
        denom = (1<<-(exp-24));
    }
    
    friend std::ostream& operator << (std::ostream& os, rational const& r)
    {
        return os << r.nom << " / " << r.denom;
    }
};

};