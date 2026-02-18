#pragma once

namespace i3
{
		
	// 다음은 비표준..
	template<class FwdIt, class Diff, class Pred1> inline
	FwdIt search_n_if(FwdIt f, FwdIt l, Diff count, Pred1 pr);

}

#include "detail/search_n_if.inl"
