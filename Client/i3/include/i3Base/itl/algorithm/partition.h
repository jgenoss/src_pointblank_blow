#pragma once

namespace i3
{
	
	template<class BidIt, class Pred1> inline
	BidIt partition(BidIt f, BidIt l, Pred1 pr);

}

#include "detail/partition.inl"