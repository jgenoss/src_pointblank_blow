#pragma once

namespace i3
{
	template <class T> struct remove_extent { typedef T type; };
	template <class T, std::size_t N> struct remove_extent<T[N]> { typedef T type; };
	template <class T, std::size_t N> struct remove_extent<T const [N]> { typedef T const type; };
	template <class T, std::size_t N> struct remove_extent<T volatile[N]> { typedef T volatile type; };
	template <class T, std::size_t N> struct remove_extent<T const volatile[N]> { typedef T const volatile type; };
	
	template <class T> struct remove_extent<T []> {	typedef T type; };
	template <class T> struct remove_extent<T const []> {	typedef T const type; };
	template <class T> struct remove_extent<T volatile []> {	typedef T volatile type; };
	template <class T> struct remove_extent<T const volatile []> {	typedef T const volatile type;  };
}

