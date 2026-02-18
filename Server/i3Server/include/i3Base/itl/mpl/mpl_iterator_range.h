#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		struct iterator_range_tag;						// 선언으로만..

		template< class First = na, class Last = na >
		struct iterator_range
		{
			typedef iterator_range_tag	tag;
			typedef iterator_range		type;
			typedef First				begin;
			typedef Last				end;
		};
		
	}
}


#define na_spec (2, iterator_range)
#include "aux_/mpl_na_spec.h"

