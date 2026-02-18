#pragma once

#include "../algorithm/detail/case_conv_fast_functor.h"
#include "../../itl/type_traits/is_signed.h"
#include "../../itl/type_traits/make_unsigned.h"

namespace i3
{
	struct is_iless
	{
		template<class T1, class T2> inline
		bool operator()(const T1& lhs, const T2& rhs)
		{
			typedef typename mpl::eval_if< is_signed<T1>, make_unsigned<T1>, identity<T1> >::type	unsigned_type1;
			typedef typename mpl::eval_if< is_signed<T2>, make_unsigned<T2>, identity<T2> >::type	unsigned_type2;
			
			return to_lower_functor_fast<unsigned_type1>()(lhs) < to_lower_functor_fast<unsigned_type2>()(rhs);
		}
		
	};

}