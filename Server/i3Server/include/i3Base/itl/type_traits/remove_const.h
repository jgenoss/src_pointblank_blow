#pragma once

#include "cv_traits_impl.h"
#include "is_volatile.h"

namespace i3
{

	namespace detail
	{
		template<class T, bool is_vol> struct remove_const_helper { typedef T volatile type; };
	
		template<class T> struct remove_const_helper<T, false> { typedef T type;  };
		
		template<class T> struct remove_const_impl
		{
			typedef typename remove_const_helper<typename cv_traits_imp_ptr<T*>::unqualified_type, is_volatile<T>::value >::type type;
		};

		template<class T> struct remove_const_impl<T&> {	typedef T&		type;	};		// 참조는 무시..가장 가까운 const만 판단한다..
		template<class T, size_t N> struct remove_const_impl<T const[N]> { typedef T	type[N]; };
		template<class T, size_t N> struct remove_const_impl<T const volatile[N]> { typedef T volatile type[N]; };
	}

	template <class T> struct remove_const{	typedef typename detail::remove_const_impl<T>::type	type; };
}
