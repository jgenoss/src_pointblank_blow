#pragma once

#include "adjust_heap_impl.h"

namespace i3
{
	namespace detail
	{
		template<class RanIt, class T> inline
		void Pop_heap(RanIt First, RanIt Last, RanIt Dest, T Val )
		{	// pop *_First to *_Dest and reheap, using operator<
			*Dest = *First;
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Adjust_heap(First, Diff(0), Diff(Last - First), Val);
		}

		template<class RanIt, class T, class Pred> inline
		void Pop_heap(RanIt First, RanIt Last, RanIt Dest, T Val, Pred pred )
		{	// pop *_First to *_Dest and reheap, using operator<
			*Dest = *First;
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Adjust_heap(First, Diff(0), Diff(Last - First), Val, pred);
		}

	}
}