
#include "pop_heap_impl.h"

namespace i3
{

	namespace detail
	{
		template<class RanIt> inline
		void Pop_heap_0(RanIt First, RanIt Last )
		{	// pop *_First to *(_Last - 1) and reheap, using operator<
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;
			Pop_heap(First, Last - 1, Last - 1, value_type(*(Last - 1) )  );
		}

		template<class RanIt, class Pred> inline
		void Pop_heap_0(RanIt First, RanIt Last, Pred pred )
		{	// pop *_First to *(_Last - 1) and reheap, using operator<
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;
			Pop_heap(First, Last - 1, Last - 1, value_type(*(Last - 1) ), pred  );
		}
	
	}

	template<class RanIt> inline
	void pop_heap(RanIt First, RanIt Last)
	{	// pop *_First to *(_Last - 1) and reheap, using operator<
		if (1 < Last - First)
			detail::Pop_heap_0(First, Last);
	}

	template<class RanIt, class Pred> inline
	void pop_heap(RanIt First, RanIt Last, Pred pred)
	{	// pop *_First to *(_Last - 1) and reheap, using _Pred
		if (1 < Last - First)
			detail::Pop_heap_0(First, Last, pred);
	}
}
