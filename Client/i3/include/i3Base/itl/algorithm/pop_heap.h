#pragma once

namespace i3
{
	template<class RanIt> inline
	void pop_heap(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void pop_heap(RanIt First, RanIt Last, Pred pred);

}

#include "detail/pop_heap.inl"
