#pragma once

#include "../algorithm/detail/case_conv_fast_functor.h"

namespace i3
{
	// equal_to와 비슷하지만, 타입자체가 템플릿이 아니다..
	struct is_iequal
	{
		template<class T1, class T2> bool operator()(const T1& lhs, const T2& rhs) const 
		{ 
			return to_upper_functor_fast<T1>()(lhs) == to_upper_functor_fast<T2>()(rhs); 
		}
	};

}
