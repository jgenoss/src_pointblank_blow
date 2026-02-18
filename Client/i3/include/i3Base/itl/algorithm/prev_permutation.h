#pragma once

namespace i3
{

	template<class BidIt> inline
	bool prev_permutation(BidIt f, BidIt l);

	template<class BidIt, class Pred2> inline
	bool prev_permutation(BidIt f, BidIt l, Pred2 pr);

}

#include "detail/prev_permutation.inl"

