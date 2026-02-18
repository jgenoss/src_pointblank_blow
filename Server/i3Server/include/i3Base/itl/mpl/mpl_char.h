#pragma once

#include "mpl_char_fwd.h"
#include "mpl_integral_c_tag.h"

namespace i3
{
	namespace mpl
	{
		template<char N>
		struct char_
		{
			static const	char									value = N;
			typedef			char_									type;
			typedef			char									value_type;
			typedef			integral_c_tag							tag;
			typedef			char_<static_cast<char>(value+1)>		next;
			typedef			char_<static_cast<char>(value-1)>		prior;
			operator		char() const			{ return this->value; }
		};
		

	}
}