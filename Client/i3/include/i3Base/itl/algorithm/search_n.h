#pragma once

namespace i3
{
	template<class FwdIt, class Diff, class T> inline
	FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val);
	
	template<class FwdIt, class Diff, class T, class Pred2> inline
	FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, Pred2 pr);
	

}

#include "detail/search_n.inl"