#pragma once

namespace i3
{
	template<class InIt1, class InIt2, class OutIt> inline
	OutIt set_union(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, OutIt dest);

	template<class InIt1, class InIt2, class OutIt, class Pred2> inline
	OutIt set_union(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, OutIt dest, Pred2 pr);
	
}

#include "detail/set_union.inl"