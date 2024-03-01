#pragma once

#include <defines.hpp>
#include <string>

#include <bit>

namespace er
{

using digit  = u32;
using digit2 = u64;

struct num
{
    static constexpr u64   bits = 32;
    static constexpr digit mask = ~digit(0u);

    digit extension = 0;
    std::vector<digit> data;
    
    constexpr num() = default;

    template<class T> requires(std::is_integral_v<T>)
    constexpr num(T n)
    {
        if(n < 0)
			extension = mask;
	    
        for (int i = 0; i < (sizeof(T) * 8); i += bits)
            data.push_back((n >> i) & mask);

        canonicalize();
    }
    
    digit get(size_t i) const
    {
        return i < data.size() ? data[i] : extension;
    }

    bool is_cannon() const
    {
        return data.empty() || data.back() != extension;
    }

    void canonicalize()
	{
		while(!data.empty() && data.back() == extension)
			data.pop_back();
	}

    static digit adc(digit a, digit b, digit& c)
    {
        digit2 r = digit2(a & mask) + digit2(b & mask) + digit2(c & 1);
        c = (r >> bits) & 1;
        return r & mask;
    }

    friend num operator ~(num const& n)
    {
        num res = n;
		for (auto& d : res.data)
			d = ~d;
		res.extension = (~res.extension) & mask;
		res.canonicalize();
		return res;
    }

    friend num operator - (num const& n)
	{
		return ~n + 1;
	}

    friend bool operator == (num const& l, num const& r)
    {
        assert(l.is_cannon() && r.is_cannon());
        return l.extension == r.extension && l.data == r.data;
    }

    friend bool operator != (num const& l, num const& r)
    {
        return !(l==r);
    }
    
    friend num operator - (num const& l, num const& r)
    {
        return l + (-r);
    }

    friend num operator + (num const& l, num const& r)
    {
        const size_t len = std::max(l.data.size(), r.data.size()) + 1;
        num res;
        res.data.resize(len);
        digit carry = 0;
        for (int i = 0; i < len; ++i)
            res.data[i] = adc(l.get(i), r.get(i), carry);

        if (l.extension == r.extension)
        {
            res.extension = l.extension;
            if(carry && 0 == res.extension)
                res.data.push_back(carry);
        }
        else
            res.extension = carry ? digit(0) : mask;

        res.canonicalize();
        return res;
    }

    friend num abs(num const& n)
    {
        return n.extension ? -n : n;
    }

    friend num operator *(num const& l, num const& r)
    {
        if(l.extension && r.extension)
            return -l * -r;
        if(l.extension)
            return -(-l * r);
        if(r.extension)
            return -(l * -r);

        num res;
		res.data.resize(l.data.size() + r.data.size());
		for (size_t i = 0; i < l.data.size(); ++i)
		{
			digit carry = 0;
			for (size_t j = 0; j < r.data.size(); ++j)
			{
				digit2 t = digit2(l.data[i]) * digit2(r.data[j]) + digit2(res.data[i + j]) + carry;
				res.data[i + j] = t & mask;
				carry = (t >> bits) & mask;
			}
			res.data[i + r.data.size()] = carry;
		}
        res.extension = (l.extension ^ r.extension) & mask;
		res.canonicalize();
		return res;
    }

    static int num_bits(digit a)
    {
        static constexpr int ignored_bits = sizeof(digit) * 8 - bits;
        return int(bits + ignored_bits) - std::countl_zero(a);
    }

    static int num_bits(num const& n)
    {
        assert(!n.extension);
        if(n.data.empty())
            return 0;
        return (n.data.size() - 1) * bits + num_bits(n.data.back());
    }

    friend bool operator <(num const& l, num const& r)
    {
        if(l.extension && r.extension)
			return -l > -r;
		if(l.extension)
			return true;
		if(r.extension)
			return false;

		if(l.data.size() != r.data.size())
			return l.data.size() < r.data.size();

		for (int i = l.data.size() - 1; i >= 0; --i)
		{
			if(l.data[i] != r.data[i])
				return l.data[i] < r.data[i];
		}
		return false;
    }

    friend bool operator >(num const& l, num const& r)
    {
		if(l.extension && r.extension)
			return -l < -r;
		if(l.extension)
			return false;
		if(r.extension)
			return true;

		if(l.data.size() != r.data.size())
			return l.data.size() > r.data.size();

		for (int i = l.data.size() - 1; i >= 0; --i)
		{
			if(l.data[i] != r.data[i])
				return l.data[i] > r.data[i];
		}
		return false;
    }

    static int bit_diff(num const& l, num const& r)
    {
        return num_bits(l) - num_bits(r);
    }

    friend num& operator -=(num& l, num const& r) { return l = l - r; }
    friend num& operator +=(num& l, num const& r) { return l = l + r; }

    friend num operator/(num const& a, num const& b)
    {
        num r;
        return divmod(a, b, r);
    }

    friend num operator%(num const& a, num const& b)
    {
        num r;
        divmod(a, b, r);
        return r;
    }

    friend num gcd(num const& a, num const& b)
    {
        if(a.extension && b.extension) return gcd(-a, -b);
        if(a.extension) return gcd(-a, b);
        if(b.extension) return gcd(a, -b);

        if(0 == b) return a;
        if(0 == a) return b;
        return gcd(b, a % b);
    }

    friend num divmod(num const& a, num const& b, num& r)
    {
        if (a.extension && b.extension)
        {
            auto q = divmod(-a, b, r);
            r = -r;
            return q;
        }
        if (a.extension)
        {
            auto q = divmod(-a, b, r);
            r = -r;
            return -q;
        }

        if (b.extension)
        {
			return -divmod(a, -b, r);
		}

        if (a == b)
        {
			r = 0;
			return 1;
		}

        if (a < b)
        {
			r = a;
			return 0;
		}
        if (b == 1)
        {
            r = a;
            return a;
        }

        num p = b;
        num q = 0;
        r = a;

        while (r > p || r == p)
        {
            const int d = 1u << std::max(0, bit_diff(r,p) - 1);
            if (d > 1)
                r -= p * d;
            else 
                r -= p;
            q += d;
        }

        return q;
    }


    friend std::string as_decimal_string(num const& n)
    {
        if(n.extension)
			return "-" + as_decimal_string(-n);
		if(n.data.empty())
			return "0";

		std::string res;
		num r = n;
        num d;
        while (r > 0)
        {
            r = divmod(r, 10, d);
			res = char('0' + d.get(0)) + res;
		}
		return res;
    }

    friend std::ostream& operator << (std::ostream& os, num const& n)
	{
        return os << as_decimal_string(n);
        if (n.extension)
        {
            auto what = -n;
            assert(!what.extension);
			os << "-" << what;
            return os;
        }

        bool w = false;
        for (int i = n.data.size() - 1; i >= 0; --i)
        {
            if(!w && n.data[i] == 0)
				continue;
            if (bits < 4)
            {
                for (int j = bits - 1; j >= 0; --j)
                {
                    w = true;
                    os << "01"[n.data[i] >> j & 1];
                }
            }
            else
            {
                for (int j = bits - 4; j >= 0; j -= 4)
                {
                    u32 idx = (n.data[i] >> j) & 15;
                    if (!w && idx == 0)
                        continue;
                    w = true;
                    os << "0123456789ABCDEF"[idx];
                }
            }
        }

        if(!w)
            os << "0";
        return os;
	}
};

}