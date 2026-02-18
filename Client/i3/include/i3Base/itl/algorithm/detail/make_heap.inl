
#include "adjust_heap_impl.h"

namespace i3
{

	template<class RanIt> inline
	void make_heap(RanIt First, RanIt Last)
	{	// make [_First, _Last) into a heap, using operator<
		if (1 < Last - First)
		{
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;

			Diff Bottom = Last - First;

			for (Diff Hole = Bottom / 2; 0 < Hole; )
			{	// reheap top half, bottom to top
				--Hole;
				detail::Adjust_heap(First, Hole, Bottom, value_type(*(First + Hole) ) );
			}
		}
	}

	template<class RanIt, class Pred> inline
	void make_heap(RanIt First, RanIt Last, Pred pred)
	{	// make [_First, _Last) into a heap, using operator<
		if (1 < Last - First)
		{
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;

			Diff Bottom = Last - First;

			for (Diff Hole = Bottom / 2; 0 < Hole; )
			{	// reheap top half, bottom to top
				--Hole;
				detail::Adjust_heap(First, Hole, Bottom, value_type(*(First + Hole) ), pred );
			}
		}
	}


}
