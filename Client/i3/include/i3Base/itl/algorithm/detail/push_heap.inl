
#include "push_heap_impl.h"

namespace i3
{

	namespace detail
	{
	
		template<class RanIt> inline
		void Push_heap_0(RanIt First, RanIt Last )
		{	// push *_Last onto heap at [_First, _Last), using operator<
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Diff Count = Last - First;
			if ( Count > 0 )		
			{
				typedef typename i3::iterator_traits<RanIt>::value_type value_type;
				Push_heap(First, Count, 0, value_type(*Last));
			}
		}

		template<class RanIt, class Pred> inline
		void Push_heap_0(RanIt First, RanIt Last, Pred pred )
		{	// push *_Last onto heap at [_First, _Last), using _Pred
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Diff Count = Last - First;
			if ( Count > 0 )		
			{
				typedef typename i3::iterator_traits<RanIt>::value_type value_type;
				Push_heap(First, Count, 0, value_type(*Last), pred);			
			}
		}

	}


	template<class RanIt> inline
	void push_heap(RanIt First, RanIt Last)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using operator<
		if (First != Last) 	// check and push to nontrivial heap
			detail::Push_heap_0(First, --Last);
	}

	template<class RanIt, class Pred> inline
	void push_heap(RanIt First, RanIt Last, Pred pred)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using _Pred
		if (First != Last)	// check and push to nontrivial heap
			detail::Push_heap_0(First, --Last, pred );
	}

}
