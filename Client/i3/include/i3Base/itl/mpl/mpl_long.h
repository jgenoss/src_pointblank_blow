#pragma once

#include "mpl_long_fwd.h"
#include "mpl_integral_c_tag.h"

namespace i3
{
	namespace mpl
	{
		template<long l>
		struct long_
		{
			static	const long			value = l;
			typedef long_				type;
			typedef long				value_type;
			typedef integral_c_tag		tag;
			typedef long_<static_cast<long>(value + 1)>	next;
			typedef long_<static_cast<long>(value - 1)>	prior;
			operator long () const		{ return this->value; }
		};
	}
}
