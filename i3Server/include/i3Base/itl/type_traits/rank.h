#pragma once

#include "integral_constant.h"

namespace i3
{
	namespace detail
	{
		template <class T, std::size_t N> struct rank_imp {   static const std::size_t value = N;	};
		template <class T, std::size_t R, std::size_t N> struct rank_imp<T[R], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t R, std::size_t N> struct rank_imp<T const[R], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t R, std::size_t N> struct rank_imp<T volatile[R], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t R, std::size_t N> struct rank_imp<T const volatile[R], N>  { static const std::size_t value = rank_imp<T, N+1>::value; };
		
		template <class T, std::size_t N> struct rank_imp<T[], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t N> struct rank_imp<T const[], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t N> struct rank_imp<T volatile[], N>  { static const std::size_t value = rank_imp<T, N+1>::value; };
		template <class T, std::size_t N> struct rank_imp<T const volatile[], N> { static const std::size_t value = rank_imp<T, N+1>::value; };
	}

	template<class T> struct rank : integral_constant<std::size_t, detail::rank_imp<T, 0>::value > {};
}
