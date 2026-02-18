#pragma once


namespace i3
{
	template<class BidIt> inline
	void inplace_merge(BidIt f, BidIt m, BidIt l);
	
	template<class BidIt, class Pred2> inline
	void inplace_merge(BidIt f, BidIt m, BidIt l, Pred2 pr);

}

#include "detail/inplace_merge.inl"