#pragma once

#include "../algorithm/detail/case_conv_fast_functor.h"

namespace i3
{
	struct is_equal_filechar
	{
		template<class T1, class T2> bool operator()(const T1& lhs, const T2& rhs) const
		{
			T1 lhs2 = (lhs == T1('\\')) ? T1('/') : to_upper_functor_fast<T1>()(lhs);
			T2 rhs2 = (rhs == T2('\\')) ? T2('/') : to_upper_functor_fast<T2>()(rhs);

			return lhs2 == rhs2;
		}
	};

}