#pragma once

namespace i3
{
	template<class InIt1, class InIt2> inline
	bool lexicographical_compare(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2);

	template<class InIt1, class InIt2, class Pred2> inline
	bool lexicographical_compare(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, Pred2 pr);
	
}

#include "detail/lexicographical_compare.inl"