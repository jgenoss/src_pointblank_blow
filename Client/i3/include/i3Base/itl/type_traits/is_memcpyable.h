#pragma once

#include "has_trivial_copy.h"
#include "has_trivial_assign.h"
#include "remove_pointer.h"
#include "is_same.h"
#include "../mpl/mpl_identity.h"

#include "make_unsigned.h"
#include "is_integral.h"

namespace i3
{
	//
	// 매개변수 in == out , in != out
	// 두가지버전이 필요.. 
	// memcpy류 함수의 인수인 포인터타입이 아닌 가르키는 포인티 타입으로 검사한다..

	//
	template<class T>
	struct is_trivially_copyable : mpl::and_< has_trivial_copy<T>, has_trivial_assign<T> > {};

	template<class T>
	struct is_memcpyable : is_trivially_copyable<T> {};
	//
	

	//
	// 1. From은 const/non_const가 모두 가능해야하고, To는 non_const여야 함... (첫번째것이 제약이 된다..)
	// 2. T*의 경우, To쪽이 const T*로의 카피가 가능해야함..
	//    
	namespace detail
	{
		// T**를 제외하고는 정수형타입의 경우 signed/unsigned 구별을 하지 않도록 조정해본다.
		
		template<class T>
		struct adjust_integral_to_unsigned
		{
			typedef typename mpl::eval_if< mpl::and_<is_integral<T>, mpl::not_< is_same<T, bool> > >, make_unsigned<T>,
				mpl::identity<T> >::type type;
		};
		

		template<class F, class T>
		struct is_memcpyable_f_t_impl
		{
			typedef typename i3::remove_const<F>::type			nc_from_type;
			typedef T											nc_to_type;		// T는 non_const여야 한다..
			
			typedef typename mpl::eval_if< is_pointer<nc_from_type>, remove_pointer<nc_from_type>, 
				adjust_integral_to_unsigned<nc_from_type> >::type check_from_type;
			typedef typename mpl::eval_if< is_pointer<nc_to_type>, 
				remove_const<typename remove_pointer<nc_to_type>::type>, 
				adjust_integral_to_unsigned<nc_to_type> >::type check_to_type;
			
			typedef typename mpl::and_< i3::is_same< check_from_type, check_to_type>, 
										i3::is_memcpyable<nc_from_type> >::type type;
		};

	}

	template<class From, class To>
	struct is_memcpyable_from_to : mpl::eval_if< is_const<To>, false_type, detail::is_memcpyable_f_t_impl<From, To> >::type
	{
	};
	
	
}