
#include "../pop_heap.h"

namespace i3
{

	template<class RanIt> inline
	void sort_heap(RanIt First, RanIt Last)
	{	// order heap by repeatedly popping, using operator<
		for (; 1 < Last - First; --Last)
			i3::pop_heap(First, Last);
	}

	template<class RanIt, class Pred> inline
	void sort_heap(RanIt First, RanIt Last, Pred pred)
	{	// order heap by repeatedly popping, using operator<
		for (; 1 < Last - First; --Last)
			i3::pop_heap(First, Last, pred);
	}

}
