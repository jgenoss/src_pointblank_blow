
#include "../make_heap.h"
#include "adjust_heap_impl.h"
#include "../sort_heap.h"

namespace i3
{

	template<class InIt, class RanIt> inline
	RanIt partial_sort_copy(InIt f1, InIt l1, RanIt f2, RanIt l2)
	{
		if ( f1 == l1 || f2 == l2) return f2;

		RanIt mid2 = f2;
		for (; f1 != l1 && mid2 != l2; ++f1, ++mid2)
			*mid2 = *f1;	

		i3::make_heap(f2, mid2);
		
		typedef typename i3::iterator_traits<RanIt>::difference_type difference_type;
		typedef typename i3::iterator_traits<InIt>::value_type value_type;

		for (; f1 != l1; ++f1)
			if (*f1 < *f2)
				detail::Adjust_heap(f2, difference_type(0), difference_type(mid2 - f2), value_type(*f1) );	// replace top with new largest

		i3::sort_heap(f2, mid2);
		return mid2;
	}

	template<class InIt, class RanIt, class Pred2> inline
	RanIt partial_sort_copy(InIt f1, InIt l1, RanIt f2, RanIt l2, Pred2 pr)
	{
		if ( f1 == l1 || f2 == l2) return f2;

		RanIt mid2 = f2;
		for (; f1 != l1 && mid2 != l2; ++f1, ++mid2)
			*mid2 = *f1;	

		i3::make_heap(f2, mid2, pr);

		typedef typename i3::iterator_traits<RanIt>::difference_type difference_type;
		typedef typename i3::iterator_traits<InIt>::value_type value_type;

		for (; f1 != l1; ++f1)
			if ( pr(*f1, *f2) )
				detail::Adjust_heap(f2, difference_type(0), difference_type(mid2 - f2), value_type(*f1), pr);	// replace top with new largest

		i3::sort_heap(f2, mid2, pr);
		return mid2;
	}


}