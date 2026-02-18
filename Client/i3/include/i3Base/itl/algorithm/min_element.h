#pragma once

namespace i3
{
	template<class FwdIt> inline
	FwdIt min_element(FwdIt f, FwdIt l);

	template<class FwdIt, class Pred2 > inline
	FwdIt min_element(FwdIt f, FwdIt l, Pred2 pr);
	
}

#include "detail/min_element.inl"