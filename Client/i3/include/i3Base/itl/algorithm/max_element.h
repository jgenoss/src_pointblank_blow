#pragma once

namespace i3
{
	template<class FwdIt> inline
	FwdIt max_element(FwdIt f, FwdIt l);

	template<class FwdIt, class Pred2 > inline
	FwdIt max_element(FwdIt f, FwdIt l, Pred2 pr);

}

#include "detail/max_element.inl"
