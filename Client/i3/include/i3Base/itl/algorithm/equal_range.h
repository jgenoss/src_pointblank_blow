#pragma once

namespace i3
{

	template<class forward_iterator, class T> inline
		i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val);

	template<class forward_iterator, class T, class Pred> inline
		i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val, Pred pr);

}

#include "detail/equal_range.inl"
