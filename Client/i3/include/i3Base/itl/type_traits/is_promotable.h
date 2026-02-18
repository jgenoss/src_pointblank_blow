#pragma once

//
// 오버로딩 규칙 3번째가 승격조건..
//
#include "decay.h"

namespace i3
{
	// 승격타입 자체가 먼저 존재해야함..
	namespace detail
	{
		template<class F, class T>
		struct is_promotable_impl2
		{
			typedef typename i3::promote<F>::type		promoted_type;

			static const bool value = mpl::and_< mpl::not_<is_same<F, promoted_type> >, 
				is_same<promoted_type, T> >::value;			
			typedef integral_constant<bool, value>	type;
		};
		
		template<class F, class T>
		struct is_promotable_impl
		{
			typedef typename i3::remove_cv<typename i3::decay<F>::type>::type	no_cv_F; 
			typedef	typename i3::remove_cv<typename i3::decay<T>::type>::type	no_cv_T;
		
			typedef mpl::eval_if< mpl::and_< 
									is_convertible<F, T>,
									mpl::or_< is_arithmetic<no_cv_F>, is_enum<no_cv_F> >,
									is_arithmetic<no_cv_T> >,
									is_promotable_impl2<no_cv_F, no_cv_T>,
									false_type>::type		type;
			static const bool value = type::value;
		};
	}
	
	template<class From, class To>
	struct is_promotable : integral_constant<bool, detail::is_promotable_impl<From, To>::value> {};
		
}
