#pragma once


namespace i3
{
	template<class FwdIt1, class FwdIt2> inline
	FwdIt1 find_first_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2);

	template<class FwdIt1, class FwdIt2, class Pred2> inline
	FwdIt1 find_first_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2, Pred2 pr);
	
}

#include "detail/find_first_of.inl"