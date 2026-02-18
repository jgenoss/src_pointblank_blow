
#include "../adjacent_find.h"
#include "../unique_copy.h"

namespace i3
{
	template<class forward_iterator> inline
	forward_iterator unique(forward_iterator first, forward_iterator last)
	{
		first = i3::adjacent_find(first, last);
		return i3::unique_copy(first, last, first);
	}

	template<class forward_iterator, class binary_pred> inline
	forward_iterator unique(forward_iterator first, forward_iterator last, binary_pred pr)
	{
		first = i3::adjacent_find(first, last, pr);
		return i3::unique_copy(first, last, first, pr);
	}

}

