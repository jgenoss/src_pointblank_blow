#pragma once

#include "../../algorithm/copy.h"

namespace i3
{
	namespace range
	{
		template<class SPRange, class OutIt> inline
		OutIt copy(const SPRange& rng, OutIt out)
		{
			return i3::copy(begin(rng), end(rng), out);
		}
	}
}

namespace i3range = i3::range;