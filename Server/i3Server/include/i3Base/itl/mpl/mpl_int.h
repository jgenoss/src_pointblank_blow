#pragma once

#include "mpl_int_fwd.h"
#include "mpl_integral_c_tag.h"

namespace i3
{
	namespace mpl
	{
		template<int N>
		struct int_
		{
			static const	int					value = N;
			typedef			int_				type;
			typedef			int					value_type;
			typedef			integral_c_tag		tag;
			typedef			int_<static_cast<int>(value+1)>		next;
			typedef			int_<static_cast<int>(value-1)>		prior;
			operator		int() const			{ return this->value; }
		};
		
	}
}
