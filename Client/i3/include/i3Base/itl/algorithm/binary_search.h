#pragma once

namespace i3
{

	template<class forward_iterator, class T> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val);

	template<class forward_iterator, class T, class Pred> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val, Pred pr);

}

#include "detail/binary_search.inl"