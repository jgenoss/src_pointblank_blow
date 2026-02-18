
#include "../../iter_swap.h"
#include "../reverse.h"


namespace i3
{
	template<class BidIt> inline
	bool next_permutation(BidIt f, BidIt l)
	{
		BidIt next = l;
		if (f == l || f == --next) return false;

		for (; ; )
		{	// find rightmost element smaller than successor
			BidIt next1 = next;
			if ( *--next < *next1 )
			{	// swap with rightmost element that's smaller, flip suffix
				BidIt mid = l;
				for (; !(*next < *--mid); ) ;
				i3::iter_swap(next, mid);
				i3::reverse(next1, l);
				return true;
			}

			if (next == f)
			{	// pure descending, flip all
				i3::reverse(f, l);
				return false;
			}
		}
		
	}

	template<class BidIt, class Pred2> inline
	bool next_permutation(BidIt f, BidIt l, Pred2 pr)
	{
		BidIt next = l;
		if (f == l || f == --next) return false;

		for (; ; )
		{	// find rightmost element smaller than successor
			BidIt next1 = next;
			if (pr(*--next, *next1))
			{	// swap with rightmost element that's smaller, flip suffix
				BidIt mid = l;
				for (; !pr(*next, *--mid); ) ;
				i3::iter_swap(next, mid);
				i3::reverse(next1, l);
				return true;
			}

			if (next == f)
			{	// pure descending, flip all
				i3::reverse(f, l);
				return false;
			}
		}
		
	}

}