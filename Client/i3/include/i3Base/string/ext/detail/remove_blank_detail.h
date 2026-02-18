#pragma once

namespace i3
{
	namespace detail
	{
		struct find_non_blank_pos_functor
		{
			template<class C> inline
				bool operator()(C c) {	return c > 32;	}
		};

	}
}
