#pragma once

#include "iter_split_fast.h"
#include "token_finder.h"

namespace i3
{
	template<class DestRange, class SrcRange, class Pred1> inline
	i3::vector<DestRange>& split_fast( i3::vector<DestRange>& Result, const SrcRange& Input, Pred1 pr, bool token_compress = false)
	{
		return iter_split_fast(Result, Input, token_finder(pr, token_compress) );
	}
}
