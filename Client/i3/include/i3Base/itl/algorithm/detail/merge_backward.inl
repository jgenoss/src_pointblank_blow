
#include "../copy_backward.h"

namespace i3
{

	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest)
	{	// merge backwards to _Dest, using operator<
		for ( ; ; )
			if (first1 == last1)
				return i3::copy_backward(first2, last2, dest);
			else if (first2 == last2)
				return i3::copy_backward(first1, last1, dest);
			else if (*--last2 < *--last1 )
				*--dest = *last1, ++last2;
			else
				*--dest = *last2, ++last1;
	}

	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3, class binary_pred> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest, binary_pred pr)
	{	// merge backwards to _Dest, using operator<
		for ( ; ; )
			if (first1 == last1)
				return i3::copy_backward(first2, last2, dest);
			else if (first2 == last2)
				return i3::copy_backward(first1, last1, dest);
			else if (pr(*--last2, *--last1))
				*--dest = *last1, ++last2;
			else
				*--dest = *last2, ++last1;
	}

}