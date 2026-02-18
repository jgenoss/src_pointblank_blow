#pragma once

namespace i3
{

	template<class RanIt> inline
	void sort(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void sort(RanIt First, RanIt Last, Pred pred);

}

#include "detail/sort.inl"
