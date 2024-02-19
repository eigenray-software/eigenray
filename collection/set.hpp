#pragma once

#include <defines.hpp>
#include <map>

namespace er
{

struct Folder
{
    u32 val = 0;
    friend ER_CONSTEXPR Folder operator +(bool b, Folder f) { return { b | (f.val << 1) }; }
};

template<int N, class...K>
struct Nth;

template<int N, class K, class...Ks>
struct Nth<N, K, Ks...> : Nth<N-1, Ks...> {};

template<class K, class...Ks>
struct Nth<0, K, Ks...>{ using type = K; };

template<int N, class...Ks>
using Nth_t = typename Nth<N, Ks...>::type;

template<int N>
struct Predicate
{
    template<class...T>
    ER_STATIC_CONSTEXPR bool cmp(std::tuple<T...> const& a, std::tuple<T...> const& b)
    {
        return std::get<N>(a) < std::get<N>(b);
    }

    template<class...T>
    ER_STATIC_CONSTEXPR Nth_t<N, T...> const& get(std::tuple<T...> const& a)
    {
        return std::get<N>(a);
    }

    template<class...T>
    ER_STATIC_CONSTEXPR bool cmp(std::tuple<T...> const& a, Nth_t<N, T...> const& b)
    {
        return std::get<N>(a) < b;
    }

    template<class...T>
    ER_STATIC_CONSTEXPR bool cmp(Nth_t<N, T...> const& a, std::tuple<T...> const& b)
    {
        return a < std::get<N>(b);
    }
};

template<int N, class T, template<int> class Pr>
using index_type = std::remove_cvref_t<std::invoke_result_t<decltype(Pr<N>::get), const T&>>;

template<class...>
struct all_unique : std::true_type {};


template<class T, class...Ts>
struct all_unique<T, Ts...> 
{
    ER_STATIC_CONSTEXPR bool value = (!std::is_same_v<T, Ts> && ... && all_unique<Ts...>::value);
};

template<class... T>
struct is_in 
{
    template<class H>
    ER_STATIC_CONSTEXPR bool value = !all_unique<H, T...>::value;
};

template <class... T>
struct first_of
{
    template<class>
    ER_STATIC_CONSTEXPR int value = sizeof...(T);
};

template <class A, class... T>
struct first_of<A, T...>
{
    template<class H>
    ER_STATIC_CONSTEXPR int value = std::is_same_v<H, A> ? 0 : (1 + first_of<T...>::template value<H>);
};

template<int...N>
struct Application
{
    template<template<int> class Pr, class T>
    ER_STATIC_CONSTEXPR u32 act(T const& a, T const& b)
    {
        return ((Pr<N>::cmp(a, b) + ... + Folder())).val;
    }

    template<class T, template<int> class Pr, template<class...> class F>
    using indexes_to_types = F<index_type<N, T, Pr>...>;
};

template<class T, int Dim, template<int> class Pr>
struct Set
{
    template<class K>
    ER_STATIC_CONSTEXPR bool key_is_valid = sequenced<Dim, Application>::template indexes_to_types<T, Pr, is_in>::template value<K>;

    template<class K>
    ER_STATIC_CONSTEXPR int key_index  = sequenced<Dim, Application>::template indexes_to_types<T, Pr, first_of>::template value<K>;

    ER_STATIC_CONSTEXPR bool can_differentiate_axis_by_type = sequenced<Dim, Application>::template indexes_to_types<T, Pr, all_unique>::value;

    struct Node
    {
        T* val = 0;
        std::map<u32, Node> children;
        
        u32 get_ordering(const T& x)
        {
            return sequenced<Dim, Application>::act<Pr, T>(*val, x);
        }
        
        void insert(const T& x)
        {
            if(!val)
            {
                val = new T(x);
                return;
            }
            children[get_ordering(x)].insert(x);
        }
        
        template<class K> requires (can_differentiate_axis_by_type && key_is_valid<K>)
        const T* find_by(K const& k)
        {
            if(!val)
			    return nullptr;

            if(Pr<key_index<K>>::get(*val) == k)
				return val;

            ER_STATIC_CONSTEXPR int N = key_index<K>;
            const u32 child_index = Pr<key_index<K>>::cmp(*val, k) << N;
            auto it = children.find(child_index);
            if(it == children.end())
				return nullptr;
			return it->second.find_by(k);
        }
    };
    
    Node root;

    template<class K> requires (can_differentiate_axis_by_type && key_is_valid<K>)
    const T* find_by(K const& k)
    {
        return root.find_by(k);
    }

    void insert(const T& x)
	{
		root.insert(x);
	}
};

#define COUNT_ARGS(...) COUNT_ARGS_(,##__VA_ARGS__,6,5,4,3,2,1,0)
#define COUNT_ARGS_(z,a,b,c,d,e,f,cnt,...) cnt

template<class T, u32 N>
struct type_reflection
{
    using type = void;
    ER_STATIC_CONSTEXPR size_t offset = 0;
};

#define FOR_EACH_FIELD(struct_type, index, field_name) \
template<> \
struct type_reflection<struct_type, index> \
{ \
    using type = decltype(struct_type::field_name); \
    ER_STATIC_CONSTEXPR size_t offset = offsetof(struct_type, field_name); \
};

#define PASTE1(...) __VA_ARGS__
#define PASTE0(...) PASTE1(PASTE1(PASTE1(PASTE1(__VA_ARGS__))))
#define PASTE(...) PASTE0(PASTE0(PASTE0(PASTE0(__VA_ARGS__))))
#define CONCATENATE_(x, y) x##y
#define CONCATENATE(x, y) PASTE(CONCATENATE_(x, y))

#define REFLECT_STRUCT_6(struct_type, entry, field_name, ...) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 6, field_name)) \
    PASTE(REFLECT_STRUCT_5(struct_type, entry,#__VA_ARGS__))

#define REFLECT_STRUCT_5(struct_type, entry, field_name, ...) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 5, field_name)) \
    PASTE(REFLECT_STRUCT_4(struct_type, entry,#__VA_ARGS__))

#define REFLECT_STRUCT_4(struct_type, entry, field_name, ...) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 4, field_name)) \
    PASTE(REFLECT_STRUCT_3(struct_type, entry,#__VA_ARGS__))

#define REFLECT_STRUCT_3(struct_type, entry, field_name, ...) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 3, field_name)) \
    PASTE(REFLECT_STRUCT_2(struct_type, entry,##__VA_ARGS__))

#define REFLECT_STRUCT_2(struct_type, entry, field_name, ...) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 2, field_name)) \
    PASTE(REFLECT_STRUCT_1(struct_type, entry,##__VA_ARGS__))

#define REFLECT_STRUCT_1(struct_type, entry, field_name) \
    PASTE(FOR_EACH_FIELD(struct_type, entry - 1, field_name))

#define REFLECT_STRUCT_0(struct_type)

#define REFLECT_STRUCT_(entry_macro, entry, struct_type, ...) \
    PASTE(entry_macro(struct_type, entry, ##__VA_ARGS__))

#define REFLECT_STRUCT(struct_type, ...) \
    REFLECT_STRUCT_(CONCATENATE(REFLECT_STRUCT_,COUNT_ARGS(__VA_ARGS__)), COUNT_ARGS(__VA_ARGS__), struct_type, ##__VA_ARGS__)

} // namespace er

