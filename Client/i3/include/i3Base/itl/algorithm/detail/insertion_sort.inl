
#include "../copy_backward.h"

namespace i3
{
	
	template<class BidIt> inline
	void insertion_sort(BidIt First, BidIt Last)
	{
		typedef typename i3::iterator_traits<BidIt>::value_type value_type;

		if (First != Last)
			for (BidIt Next = First; ++Next != Last; )
			{	// order next element
				BidIt Next1 = Next;
				value_type Val = *Next;

				if (Val < *First)
				{	// found new earliest element, move to front
					i3::copy_backward(First, Next, ++Next1 );
					*First = Val;
				}
				else
				{	// look for insertion point after first
					for (BidIt First1 = Next1; Val < *--First1 ; Next1 = First1)
						*Next1 = *First1;	// move hole down
					*Next1 = Val;	// insert element in hole
				}
			}

	}

	template<class BidIt, class Pred> inline
	void insertion_sort(BidIt First, BidIt Last, Pred pred)
	{

		typedef typename i3::iterator_traits<BidIt>::value_type value_type;

		if (First != Last)
			for (BidIt Next = First; ++Next != Last; )
			{	// order next element
				BidIt Next1 = Next;
				value_type Val = *Next;

				if ( pred(Val, *First) )
				{	// found new earliest element, move to front
					i3::copy_backward(First, Next, ++Next1 );
					*First = Val;
				}
				else
				{	// look for insertion point after first
					for (BidIt First1 = Next1; pred(Val, *--First1 ) ; Next1 = First1)
						*Next1 = *First1;	// move hole down
					*Next1 = Val;	// insert element in hole
				}
			}
	}

		
	
}