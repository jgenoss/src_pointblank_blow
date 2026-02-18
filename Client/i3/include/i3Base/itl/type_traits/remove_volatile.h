#pragma once

#include "cv_traits_impl.h"
#include "is_const.h"

namespace i3
{

	namespace detail
	{
		
		template<class T, bool is_con> struct remove_volatile_helper  { typedef T const type; };
		template<class T> struct remove_volatile_helper<T, false>		{ typedef T type;  };
	
		template<class T> struct remove_volatile_impl
		{
			typedef typename remove_volatile_helper< typename cv_traits_imp_ptr<T*>::unqualified_type, is_const<T>::value>::type type;
		};
			
		template<class T> struct remove_volatile_impl<T&> {	typedef T&		type;	};		// 참조는 무시..가장 가까운 volatile만 판단한다..
		template<class T, size_t N> struct remove_volatile_impl<T volatile[N]> { typedef T	type[N]; };
		template<class T, size_t N> struct remove_volatile_impl<T const volatile[N]> { typedef T const type[N]; };
	}

	template <class T> struct remove_volatile{	typedef typename detail::remove_volatile_impl<T>::type	type; };

}