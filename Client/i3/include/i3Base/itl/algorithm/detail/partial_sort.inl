
#include "../make_heap.h"
#include "pop_heap_impl.h"
#include "../sort_heap.h"

namespace i3
{
	template<class RanIt> inline
	void partial_sort(RanIt f, RanIt m, RanIt l)
	{
		i3::make_heap(f, m);
		
		for (RanIt next = m; next < l; ++next)
		{
			if ( *next < *f )
				detail::Pop_heap(f, m, next, *next);	// replace top with new largest
		}
		i3::sort_heap(f, m);
	}

	template<class RanIt, class Pred2> inline
	void partial_sort(RanIt f, RanIt m, RanIt l, Pred2 pr)
	{
		i3::make_heap(f, m, pr);

		for (RanIt next = m; next < l; ++next)
		{
			if ( pr(*next , *f ) )
				detail::Pop_heap(f, m, next, *next, pr);	// replace top with new largest
		}
		i3::sort_heap(f, m, pr);
	}

}
