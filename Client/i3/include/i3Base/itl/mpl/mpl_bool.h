#pragma once

#include "mpl_bool_fwd.h"
#include "mpl_integral_c_tag.h"

namespace i3
{
	namespace mpl
	{
		template< bool C>
		struct bool_
		{
			static const bool			value = C;
			typedef	integral_c_tag		tag;
			typedef bool_				type;
			typedef bool				value_type;
			operator bool() const		{ return this->value; }
		};
	}
}
