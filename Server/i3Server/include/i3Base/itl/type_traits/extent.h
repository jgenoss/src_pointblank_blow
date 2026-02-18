#pragma once

#include "integral_constant.h"

namespace i3
{
	
	namespace detail
	{
		template <class T, std::size_t N> struct extent_imp { static std::size_t value = 0; };
		template <class T, std::size_t R, std::size_t N> struct extent_imp<T[R], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t R, std::size_t N> struct extent_imp<T const[R], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t R, std::size_t N> struct extent_imp<T volatile[R], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t R, std::size_t N> struct extent_imp<T const volatile[R], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		
		template <class T, std::size_t R> struct extent_imp<T[R],0> { static std::size_t value = R; };
		template <class T, std::size_t R> struct extent_imp<T const[R], 0> { static std::size_t value = R; };
		template <class T, std::size_t R> struct extent_imp<T volatile[R], 0> { static std::size_t value = R; };
		template <class T, std::size_t R> struct extent_imp<T const volatile[R], 0> { static std::size_t value = R; };
  
		template <class T, std::size_t N> struct extent_imp<T[], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t N> struct extent_imp<T const[], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t N> struct extent_imp<T volatile[], N> { static std::size_t value = extent_imp<T, N-1>::value; };
		template <class T, std::size_t N> struct extent_imp<T const volatile[], N> { static std::size_t value = extent_imp<T, N-1>::value; };
   
		template <class T> struct extent_imp<T[], 0> { static std::size_t value = 0; };
		template <class T> struct extent_imp<T const[], 0> { static std::size_t value = 0; };
		template <class T> struct extent_imp<T volatile[], 0> { static std::size_t value = 0; };
		template <class T> struct extent_imp<T const volatile[], 0> { static std::size_t value = 0; };
   	}

	template <class T, std::size_t N = 0>
	struct extent : integral_constant<std::size_t, detail::extent_imp<T,N>::value> {};
	
}