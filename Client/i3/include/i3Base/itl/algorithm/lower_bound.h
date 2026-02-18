#pragma once

namespace i3
{

	template<class ForwardIterator, class T> inline
		ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val);

	template<class ForwardIterator, class T, class Pred> inline
		ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr );

}

#include "detail/lower_bound.inl"
