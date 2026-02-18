#pragma once

#include "cv_traits_impl.h"

namespace i3
{
	namespace detail
	{
		template<class T> struct remove_cv_impl {	typedef typename cv_traits_imp_ptr<T*>::unqualified_type	type; };
		template<class T> struct remove_cv_impl<T&> {	typedef T&	type; };
		template<class T, size_t N> struct remove_cv_impl<T const[N]> { typedef T	type[N]; };
		template<class T, size_t N> struct remove_cv_impl<T volatile[N]> { typedef T	type[N]; };
		template<class T, size_t N> struct remove_cv_impl<T const volatile[N]> { typedef T type[N]; };
	}

	template <class T> struct remove_cv{	typedef typename detail::remove_cv_impl<T>::type	type; };
	
}
