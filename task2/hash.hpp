#ifndef HASH_H
#define HASH_H
#include "Types.hpp"

#if 0


namespace tbb{
    namespace interface5{
        
        
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct __HashValueImpl
        {
            inline static void apply ( size_t& seed, Tuple const& tuple )
            {
                using elType = typename std::tuple_element<Index,Tuple>::type;
                __HashValueImpl<Tuple, Index-1>::apply ( seed, tuple );
                seed ^= tbb::interface5::tbb_hasher<elType> (std::get<Index> ( tuple ) );
            }
        };

        template <class Tuple>
        struct __HashValueImpl<Tuple,0>
        {
            inline static void apply ( size_t& seed, Tuple const& tuple )
            {
                using elType = typename std::tuple_element<0,Tuple>::type;
                seed ^= tbb::interface5::tbb_hasher<elType> (std::get<0> ( tuple ) );
            }
        };

        
        template <typename ...TT>
        inline size_t tbb_hasher(const std::tuple<TT...>& tt ) {
            size_t seed = 0;
            __HashValueImpl<std::tuple<TT...>>::apply(seed, tt);// * internal::hash_multiplier;
            return seed;
        }
        
    }

    template <typename ...TT>
    size_t tbb::tbb_hash<TT...>::operator()(const std::tuple<TT...>& key)
    {
        return tbb_hasher<TT...>(key);
    }
}

#endif


namespace std
{
namespace
{

// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     http://stackoverflow.com/questions/4948780

template <class T>
inline void hash_combine ( std::size_t& seed, T const& v )
{
    seed ^= hash<T>() ( v ) + 0x9e3779b9 + ( seed<<6 ) + ( seed>>2 );
}

// Recursive template code derived from Matthieu M.
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl
{
    static void apply ( size_t& seed, Tuple const& tuple )
    {
        HashValueImpl<Tuple, Index-1>::apply ( seed, tuple );
        hash_combine ( seed, get<Index> ( tuple ) );
    }
};

template <class Tuple>
struct HashValueImpl<Tuple,0>
{
    static void apply ( size_t& seed, Tuple const& tuple )
    {
        hash_combine ( seed, get<0> ( tuple ) );
    }
};
}

template <typename ... TT>
struct hash<std::tuple<TT...>>
{
    size_t
    operator() ( std::tuple<TT...> const& tt ) const
    {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply ( seed, tt );
        return seed;
    }

};
template <>
struct hash<Integer>
{
    size_t
    operator() ( Integer const& tt ) const
    {
        return std::hash<int32_t>() ( tt.value );
    }

};
template <>
struct hash<Varchar<16>>
{
    size_t
    operator() ( Varchar<16> const& tt ) const
    {
        return std::hash<string>() ( string ( tt.value ) );
    }

};

}
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
#endif