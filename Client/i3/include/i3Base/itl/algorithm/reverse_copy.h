#pragma once


namespace i3
{

	template<class BidIt, class OutIt> inline
	OutIt reverse_copy(BidIt First, BidIt Last, OutIt Dest);
}

#include "detail/reverse_copy.inl"

