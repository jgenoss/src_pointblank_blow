#pragma once

namespace i3
{
	template<class BidIt> inline
	void insertion_sort(BidIt First, BidIt Last);

	template<class BidIt, class Pred> inline
	void insertion_sort(BidIt First, BidIt Last, Pred pred);
}

#include "detail/insertion_sort.inl"
