#pragma once


namespace i3
{
	
	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest);

	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3, class binary_pred> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest, binary_pred pr);

}

#include "detail/merge_backward.inl"