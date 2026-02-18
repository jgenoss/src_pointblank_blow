#pragma once

namespace i3
{
	template<class InIt, class OutIt> inline
	OutIt	adjacent_difference(InIt f, InIt l, OutIt dest);

	template<class InIt, class OutIt, class Fn2> inline
	OutIt	adjacent_difference(InIt f, InIt l, OutIt dest, Fn2 fn);

	
}

#include "detail/adjacent_difference.inl"