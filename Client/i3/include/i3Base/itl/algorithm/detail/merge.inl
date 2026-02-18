
#include "../copy.h"

namespace i3
{

	template<class input_iterator1, class input_iterator2, class output_iterator> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest)
	{
		for ( ; first1 != last1 && first2 != last2 ; ++dest )
			if ( *first2 < *first1 )
				*dest = *first2, ++first2;
			else
				*dest = *first1, ++first1;

		dest = i3::copy(first1, last1, dest);
		return i3::copy(first2, last2, dest);
	}

	template<class input_iterator1, class input_iterator2, class output_iterator, class binary_pred> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest, binary_pred pr)
	{
		for ( ; first1 != last1 && first2 != last2 ; ++dest )
			if ( pr(*first2, *first1) )
				*dest = *first2, ++first2;
			else
				*dest = *first1, ++first1;
		dest = i3::copy(first1, last1, dest);
		return i3::copy(first2, last2, dest);
	}


}