#pragma once

namespace i3
{
	template<class RanIt> inline
	void nth_element(RanIt f, RanIt nth, RanIt l);

	template<class RanIt, class Pred2 > inline
	void nth_element(RanIt f, RanIt nth, RanIt l, Pred2 pr);
	
}

#include "detail/nth_element.inl"