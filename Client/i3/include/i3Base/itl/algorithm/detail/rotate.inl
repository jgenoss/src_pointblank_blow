
#include "../../iter_swap.h"
#include "../reverse.h"


namespace i3
{
	namespace detail
	{
		
		template<class forward_iterator> inline
		void rotate_impl( forward_iterator first, forward_iterator mid, forward_iterator last, i3::forward_iterator_tag )
		{
			for (forward_iterator next = mid; ; )
			{	// swap [_First, ...) into place
				i3::iter_swap(first, next);
				if (++first == mid)
					if (++next == last) break;	// done, quit
					else mid = next;	// mark end of next interval
				else if (++next == last)
					next = mid;	// wrap to last end
			}
		}

		template<class bidirectional_iterator> inline
		void rotate_impl(bidirectional_iterator first, bidirectional_iterator mid, bidirectional_iterator last, i3::bidirectional_iterator_tag)
		{	// rotate [_First, _Last), bidirectional iterators
			i3::reverse(first, mid);
			i3::reverse(mid, last);
			i3::reverse(first, last);
		}

		template<class random_access_iterator> inline
		void rotate_impl(random_access_iterator first, random_access_iterator mid, random_access_iterator last, i3::random_access_iterator_tag)
		{
			typedef typename i3::iterator_traits<random_access_iterator>::difference_type diff;
			typedef typename i3::iterator_traits<random_access_iterator>::value_type		T;

			diff shift = mid - first;			diff count = last - first;

			for (diff factor = shift; factor != 0; )
			{	// find subcycle count as GCD of shift count and length
				diff tmp = count % factor;
				count = factor, factor = tmp;
			}

			if (count < last - first)
			{
				for (; 0 < count; --count)
				{	// rotate each subcycle
					random_access_iterator hole = first + count;
					random_access_iterator next = hole;
					T holeval = *hole;
					random_access_iterator next1 = next + shift == last ? first : next + shift;
					while (next1 != hole)
					{	// percolate elements back around subcycle
						*next = *next1;
						next = next1;
						next1 = shift < last - next1 ? next1 + shift : first + (shift - (last - next1));
					}
					*next = holeval;
				}
			}
		}
				
	}

	template<class forward_iterator> inline
	void rotate(forward_iterator first, forward_iterator mid, forward_iterator last)
	{
		if ( first != mid && mid != last )
			detail::rotate_impl(first, mid, last, typename i3::iterator_traits<forward_iterator>::iterator_category() );
	}
}

