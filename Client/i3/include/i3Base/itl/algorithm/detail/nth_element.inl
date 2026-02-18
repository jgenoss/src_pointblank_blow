
#include "ISORT_MAX.h"
#include "unguarded_partition_impl.h"
#include "../insertion_sort.h"

namespace i3
{
	template<class RanIt> inline
	void nth_element(RanIt f, RanIt nth, RanIt l)
	{
		for (; ISORT_MAX < l - f; )
		{	// divide and conquer, ordering partition containing Nth
			i3::pair<RanIt, RanIt> mid = i3::detail::Unguarded_partition(f, l);
			if (mid.second <= nth) f = mid.second;
			else if (mid.first <= nth) return;	
			else l = mid.first;
		}

		i3::insertion_sort(f, l);	// sort any remainder
	}

	template<class RanIt, class Pred2 > inline
	void nth_element(RanIt f, RanIt nth, RanIt l, Pred2 pr)
	{
		for (; ISORT_MAX < l - f; )
		{	// divide and conquer, ordering partition containing Nth
			i3::pair<RanIt, RanIt> mid = i3::detail::Unguarded_partition(f, l, pr);
			if (mid.second <= nth) f = mid.second;
			else if (mid.first <= nth) return;	
			else l = mid.first;
		}

		i3::insertion_sort(f, l, pr);	// sort any remainder	
	}

}
