#pragma once

#include "../../algorithm/replace_copy.h"


namespace i3
{
	namespace range
	{
		template<class FwdRange, class OutIt, class Val> inline
		OutIt replace_copy(const FwdRange& rng, OutIt out_it, const Val& old_v, const Value& new_v)
		{
			return i3::replace_copy(begin(rng), end(rng), out_it, old_v, new_v );
		}		
	}
}


namespace i3range = i3::range;