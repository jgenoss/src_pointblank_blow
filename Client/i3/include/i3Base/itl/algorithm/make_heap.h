#pragma once

namespace i3
{

	template<class RanIt> inline
	void make_heap(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void make_heap(RanIt First, RanIt Last, Pred pred);

}

#include "detail/make_heap.inl"