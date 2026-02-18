#pragma once

namespace i3
{

	template<class bidirectional_iterator> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last);

	template<class bidirectional_iterator, class binary_pred> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last, binary_pred pr);

}

#include "detail/stable_sort.inl"
