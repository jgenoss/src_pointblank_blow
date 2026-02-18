#pragma once


namespace i3
{
	template<class FwdIt1, class FwdIt2> inline
	FwdIt2 swap_ranges(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2);
}

#include "detail/swap_ranges.inl"