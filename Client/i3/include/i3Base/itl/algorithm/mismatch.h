#pragma once

namespace i3
{
	template<class InIt1, class InIt2> inline
	i3::pair<InIt1, InIt2> mismatch(InIt1 f1, InIt1 l1, InIt2 f2);
	
	template<class InIt1, class InIt2, class Pred2> inline
	i3::pair<InIt1, InIt2> mismatch(InIt1 f1, InIt1 l1, InIt2 f2, Pred2 pr);

	
}

#include "detail/mismatch.inl"