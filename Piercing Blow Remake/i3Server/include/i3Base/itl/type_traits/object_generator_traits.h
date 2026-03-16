#pragma once

#include "../template_utility/ref.h"

namespace i3
{
	// boost 튜플의 어떤 구현을 기준으로..
	// 함수를 통한 템플릿 객체생성시 
	// (1) T->T 
	// (2) T& -> compile error
	// (3) reference_wrapper<T> -> T&
	// (4) const reference_wrapper<T> -> T&
	// (5) array -> const ref array
	
	template<class T>
	struct object_generator_traits { typedef T type; };
	
	namespace detail { template<class T> class generate_error; }

	template<class T>
	struct object_generator_traits<T&> { typedef typename detail::generate_error<T&>::do_not_use_with_reference_type error; };

	template<class T, int N>
	struct object_generator_traits<T[N]> { typedef const T (&type)[N]; };

	template<class T, int N>
	struct object_generator_traits<const T[N]> { typedef const T (&type)[N]; };

	template<class T, int N>
	struct object_generator_traits<volatile T[N]> { typedef const volatile T (&type)[N]; };
	
	template<class T, int N>
	struct object_generator_traits<const volatile T[N]> { typedef const volatile T (&type)[N]; };
	
	template<class T>
	struct object_generator_traits<reference_wrapper<T> > { typedef T& type; };

	template<class T>
	struct object_generator_traits<const reference_wrapper<T> > { typedef T& type; };
	
}