#pragma once

namespace i3
{
	template<class InIt, class OutIt> inline
	OutIt partial_sum(InIt f, InIt l, OutIt dest);

	template<class InIt, class OutIt, class Fn2> inline
	OutIt partial_sum(InIt f, InIt l, OutIt dest, Fn2 fn);
	
}

#include "detail/partial_sum.inl"
