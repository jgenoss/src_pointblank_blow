
#include "../../iter_swap.h"

namespace i3
{

	template<class BidIt, class Pred1> inline
	BidIt partition(BidIt f, BidIt l, Pred1 pr)
	{

		for (; ; ++f)
		{	
			for (; f != l && pr(*f); ++f);	
			if (f == l) break;	
			for (; f != --l && !pr(*l); );	
			if (f == l)	break;	

			i3::iter_swap(f, l);	
		}
		return f;
	}

}