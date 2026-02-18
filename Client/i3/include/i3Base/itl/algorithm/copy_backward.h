#pragma once

namespace i3
{
	template <class BidIt1, class BidIt2> inline
	BidIt2 copy_backward( BidIt1 f, BidIt1 l, BidIt2 dest);

	template<class BidIt1, class BidIt2> inline
	BidIt2 copy_backward_1(BidIt1 f, BidIt2 dest);
}

#include "detail/copy_backward.inl"