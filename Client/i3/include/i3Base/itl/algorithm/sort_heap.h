#pragma once

namespace i3
{

	template<class RanIt> inline
	void sort_heap(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void sort_heap(RanIt First, RanIt Last, Pred pred);

}

#include "detail/sort_heap.inl"
