#pragma once

#include "push_heap_impl.h"

namespace i3
{
	namespace detail
	{

		template<class RanIt, class Diff,	class T> inline
		void Adjust_heap(RanIt First, Diff Hole, Diff Bottom, T Val)
		{	// percolate _Hole to _Bottom, then push val, using operator<
			Diff Top = Hole;
			Diff Idx = 2 * Hole + 2;

			for (; Idx < Bottom; Idx = 2 * Idx + 2)
			{	// move _Hole down to larger child
				if ( *(First + Idx) < *(First + (Idx - 1) ) )
					--Idx;
				*(First + Hole) = *(First + Idx), Hole = Idx;
			}

			if (Idx == Bottom)
			{	// only child at bottom, move _Hole down to it
				*(First + Hole) = *(First + (Bottom - 1));
				Hole = Bottom - 1;
			}

			Push_heap(First, Hole, Top, Val);
		}

		template<class RanIt, class Diff, class T, class Pred> inline
		void Adjust_heap(RanIt First, Diff Hole, Diff Bottom, T Val, Pred pred)
		{	// percolate _Hole to _Bottom, then push val, using operator<
			Diff Top = Hole;
			Diff Idx = 2 * Hole + 2;

			for (; Idx < Bottom; Idx = 2 * Idx + 2)
			{	// move _Hole down to larger child
				if ( pred( *(First + Idx) , *(First + (Idx - 1) ) ) )
					--Idx;
				*(First + Hole) = *(First + Idx), Hole = Idx;
			}

			if (Idx == Bottom)
			{	// only child at bottom, move _Hole down to it
				*(First + Hole) = *(First + (Bottom - 1));
				Hole = Bottom - 1;
			}

			Push_heap(First, Hole, Top, Val, pred);
		}

	}
}