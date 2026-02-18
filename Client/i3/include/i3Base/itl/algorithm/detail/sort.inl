
#include "ISORT_MAX.h"

#include "unguarded_partition_impl.h"
#include "../make_heap.h"
#include "../sort_heap.h"
#include "../insertion_sort.h"

namespace i3
{

	namespace detail
	{

		template<class RanIt, class Diff> inline
		void Sort(RanIt First, RanIt Last, Diff Ideal)
		{	// order [_First, _Last), using operator<
			Diff Count;
			for (; ISORT_MAX < (Count = Last - First) && 0 < Ideal; )
			{	// divide and conquer by quicksort
				i3::pair<RanIt, RanIt> Mid = Unguarded_partition(First, Last);
				Ideal /= 2, Ideal += Ideal / 2;	// allow 1.5 log2(N) divisions

				if (Mid.first - First < Last - Mid.second)
				{	// loop on second half
					Sort(First, Mid.first, Ideal);
					First = Mid.second;
				}
				else
				{	// loop on first half
					Sort(Mid.second, Last, Ideal);
					Last = Mid.first;
				}
			}

			if (ISORT_MAX < Count)
			{	// heap sort if too many divisions
				i3::make_heap(First, Last);
				i3::sort_heap(First, Last);
			}
			else if (1 < Count)
				i3::insertion_sort(First, Last);	// small
		}

		template<class RanIt, class Diff, class Pred> inline
		void Sort(RanIt First, RanIt Last, Diff Ideal, Pred pred)
		{	// order [_First, _Last), using operator<
			Diff Count;
			for (; ISORT_MAX < (Count = Last - First) && 0 < Ideal; )
			{	// divide and conquer by quicksort
				i3::pair<RanIt, RanIt> Mid = Unguarded_partition(First, Last, pred);
				Ideal /= 2, Ideal += Ideal / 2;	// allow 1.5 log2(N) divisions

				if (Mid.first - First < Last - Mid.second)
				{	// loop on second half
					Sort(First, Mid.first, Ideal, pred);
					First = Mid.second;
				}
				else
				{	// loop on first half
					Sort(Mid.second, Last, Ideal, pred);
					Last = Mid.first;
				}
			}

			if (ISORT_MAX < Count)
			{	// heap sort if too many divisions
				i3::make_heap(First, Last, pred);
				i3::sort_heap(First, Last, pred);
			}
			else if (1 < Count)
				i3::insertion_sort(First, Last, pred);	// small
		}
		
	}

	template<class RanIt> inline
	void sort(RanIt First, RanIt Last)
	{	
		detail::Sort(First, Last, Last - First);
	}

	template<class RanIt, class Pred> inline
	void sort(RanIt First, RanIt Last, Pred pred)
	{	
		detail::Sort(First, Last, Last - First, pred);
	}

}
