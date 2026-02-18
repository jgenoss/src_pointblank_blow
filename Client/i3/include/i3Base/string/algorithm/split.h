#pragma once

#include "iter_split.h"
#include "token_finder.h"

namespace i3
{
	template<class SeqSeq, class Range, class Pred1> inline
	SeqSeq& split( SeqSeq& Result, const Range& Input, Pred1 pr, bool token_compress = false)
	{
		return iter_split(Result, Input, token_finder(pr, token_compress) );
	}
		
}