#pragma once

namespace i3
{
	template<class BidIt> inline
	bool next_permutation(BidIt f, BidIt l);

	template<class BidIt, class Pred2> inline
	bool next_permutation(BidIt f, BidIt l, Pred2 pr);

}

#include "detail/next_permutation.inl"
