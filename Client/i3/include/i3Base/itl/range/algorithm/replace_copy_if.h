#pragma once

#include "../../algorithm/replace_copy_if.h"


namespace i3
{
	namespace range
	{
		template<class FwdRange, class OutIt, class Pred1, class Val> inline
		OutIt replace_copy_if(const FwdRange& rng, OutIt out_it, Pred1 pr, const Value& new_v)
		{
			return i3::replace_copy_if(begin(rng), end(rng), out_it, pr, new_v );
		}		
	}
}

namespace i3range = i3::range;
