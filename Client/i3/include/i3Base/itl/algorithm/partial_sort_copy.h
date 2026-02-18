#pragma once

namespace i3
{

	template<class InIt, class RanIt> inline
	RanIt partial_sort_copy(InIt f1, InIt l1, RanIt f2, RanIt l2);

	template<class InIt, class RanIt, class Pred2> inline
	RanIt partial_sort_copy(InIt f1, InIt l1, RanIt f2, RanIt l2, Pred2 pr);


}

#include "detail/partial_sort_copy.inl"
