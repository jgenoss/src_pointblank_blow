#pragma once

namespace i3
{
	
	template<class T> struct remove_all_extents { typedef T type; };
	
	template<class T, std::size_t N> struct remove_all_extents<T[N]> { typedef typename remove_all_extents<T>::type type; };
	template<class T, std::size_t N> struct remove_all_extents<T const [N]> { typedef typename remove_all_extents<T const>::type type; };
	template<class T, std::size_t N> struct remove_all_extents<T volatile [N]> { typedef typename remove_all_extents<T volatile>::type type; };
	template<class T, std::size_t N> struct remove_all_extents<T const volatile [N]> { typedef typename remove_all_extents<T const volatile>::type type; };

	template<class T> struct remove_all_extents<T[]> { typedef typename remove_all_extents<T>::type type; };
	template<class T> struct remove_all_extents<T const []> { typedef typename remove_all_extents<T const>::type type; };
	template<class T> struct remove_all_extents<T volatile []> { typedef typename remove_all_extents<T volatile>::type type; };
	template<class T> struct remove_all_extents<T const volatile []> { typedef typename remove_all_extents<T const volatile>::type type; };

}
