#pragma once

namespace i3
{
	namespace detail
	{
		
		template<class RanIt, class Diff, class T> inline
		void Push_heap(RanIt First, Diff Hole, Diff Top, T Val)
		{	// percolate _Hole to _Top or where val belongs, using operator<
			for (Diff Idx = (Hole - 1) / 2; Top < Hole && *(First + Idx) < Val ; Idx = (Hole - 1) / 2)
			{	// move _Hole up to parent
				*(First + Hole) = *(First + Idx);
				Hole = Idx;
			}
			*(First + Hole) = Val;	// drop val into final hole
		}

		template<class RanIt, class Diff, class T, class Pred> inline
		void Push_heap(RanIt First, Diff Hole, Diff Top, T Val, Pred pred)
		{	// percolate _Hole to _Top or where val belongs, using operator<
			for (Diff Idx = (Hole - 1) / 2; Top < Hole &&  pred( *(First + Idx) , Val) ; Idx = (Hole - 1) / 2)
			{	// move _Hole up to parent
				*(First + Hole) = *(First + Idx);
				Hole = Idx;
			}
			*(First + Hole) = Val;	// drop val into final hole
		}

	}
}
