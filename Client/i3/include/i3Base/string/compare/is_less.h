#pragma once

namespace i3
{
	struct is_less
	{
		template<class T1, class T2> inline
		bool operator()(const T1& lhs, const T2& rhs)
		{
			return lhs < rhs;
		}
	};
		
}