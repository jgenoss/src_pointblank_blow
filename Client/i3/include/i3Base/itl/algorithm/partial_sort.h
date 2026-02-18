#pragma once

namespace i3
{
	template<class RanIt> inline
	void partial_sort(RanIt f, RanIt m, RanIt l);

	template<class RanIt, class Pred2> inline
	void partial_sort(RanIt f, RanIt m, RanIt l, Pred2 pr);

}

#include "detail/partial_sort.inl"
