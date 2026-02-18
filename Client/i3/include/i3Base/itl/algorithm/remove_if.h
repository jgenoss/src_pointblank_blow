#pragma once


namespace i3
{
	template<class FwdIt, class Pred> inline
	FwdIt	remove_if(FwdIt f, FwdIt l, Pred pr);
}

#include "detail/remove_if.inl"
