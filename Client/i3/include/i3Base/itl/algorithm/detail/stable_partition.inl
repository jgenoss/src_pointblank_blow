
#include "../../iterator.h"
#include "../../detail/temp_iterator.h"
#include "../copy.h"
#include "buffered_rotate.h"

namespace i3
{
	namespace detail
	{
		template<class BidIt, class Pred1, class Diff, class T> inline
		BidIt stable_partition(BidIt f, BidIt l, Pred1 pr, Diff count, i3::temp_pool_iterator<T>& temp_buff)
		{
			if (count == 0) {	return f;  }
			else if (count == 1) { return (pr(*f)) ? l : f; }
			else if (count <= temp_buff.max_length())
			{	// temp buffer big enough, copy right partition out and back
				BidIt next = f;
				for (temp_buff.init(); f != l; ++f)
					if (pr(*f))
						*next++ = *f;
					else
						*temp_buff++ = *f;

				i3::copy(temp_buff.first(), temp_buff.last(), next); // copy back
				return next;
			}
			else
			{	// temp buffer not big enough, divide and conquer
				BidIt mid = f;
				i3::advance(mid, count / 2);

				BidIt left = detail::stable_partition(f, mid, pr, count / 2, temp_buff);	// form L1R1 in left half
				BidIt right = detail::stable_partition(mid, l, pr, count - count / 2, temp_buff);	// form L2R2 in right half

				Diff count1 = 0; i3::distance(left, mid, count1);
				Diff count2 = 0; i3::distance(mid, right, count2);

				return detail::buffered_rotate(left, mid, right, count1, count2, temp_buff);	// rotate L1R1L2R2 to L1L2R1R2
			}
						
		}

	}


	template<class BidIt, class Pred1> inline
	BidIt stable_partition(BidIt f, BidIt l, Pred1 pr)
	{
		if ( f == l) return f;

		typedef typename i3::iterator_traits<BidIt>::value_type value_type;
		typedef typename i3::iterator_traits<BidIt>::difference_type difference_type;

		difference_type count = 0;
		i3::distance(f, l, count);
		i3::temp_pool_iterator<value_type> temp_buff(count);
		return detail::stable_partition(f, l, pr, count, temp_buff);
	}

}
