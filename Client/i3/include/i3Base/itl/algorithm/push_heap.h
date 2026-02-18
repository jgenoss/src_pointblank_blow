#pragma once

namespace i3
{

	template<class RanIt> inline
	void push_heap(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void push_heap(RanIt First, RanIt Last, Pred pred);


}

#include "detail/push_heap.inl"