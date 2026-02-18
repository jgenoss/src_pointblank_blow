#pragma once


namespace i3
{

	template<class forward_iterator> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last);

	template<class forward_iterator, class binary_pred> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last, binary_pred pr);

}

#include "detail/adjacent_find.inl"
