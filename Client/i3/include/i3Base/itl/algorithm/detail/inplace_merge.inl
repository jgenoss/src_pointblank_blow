
#include "../../detail/temp_iterator.h"
#include "../../iterator.h"
#include "buffered_merge.h"

namespace i3
{

	template<class BidIt> inline
	void inplace_merge(BidIt f, BidIt m, BidIt l)
	{
		if (f == m || m == l) return;
		
		typedef typename i3::iterator_traits<BidIt>::difference_type	difference_type;
		typedef typename i3::iterator_traits<BidIt>::value_type			value_type;

		difference_type count1 = 0;  i3::distance(f, m, count1);
		difference_type count2 = 0;  i3::distance(m, l, count2);

		i3::temp_pool_iterator<value_type> temp_buff(count1 < count2 ? count1 : count2);
		detail::buffered_merge(f, m, l, count1, count2, temp_buff);
	}

	template<class BidIt, class Pred2> inline
	void inplace_merge(BidIt f, BidIt m, BidIt l, Pred2 pr)
	{
		if (f == m || m == l) return;

		typedef typename i3::iterator_traits<BidIt>::difference_type	difference_type;
		typedef typename i3::iterator_traits<BidIt>::value_type			value_type;

		difference_type count1 = 0;  i3::distance(f, m, count1);
		difference_type count2 = 0;  i3::distance(m, l, count2);

		i3::temp_pool_iterator<value_type> temp_buff(count1 < count2 ? count1 : count2);
		detail::buffered_merge(f, m, l, count1, count2, temp_buff, pr);
	}

}
