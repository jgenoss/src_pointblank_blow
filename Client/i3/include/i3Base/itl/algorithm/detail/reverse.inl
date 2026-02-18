
#include "../../iter_swap.h"

namespace i3
{
	namespace detail
	{
		template<class BidIt> inline
		void Reverse(BidIt First, BidIt Last, i3::bidirectional_iterator_tag)
		{
			for (; First != Last && First != --Last; ++First)
				i3::iter_swap(First, Last);
		}

		template<class BidIt> inline
		void Reverse(BidIt First, BidIt Last, i3::random_access_iterator_tag)
		{
			for (; First < Last; ++First)
				i3::iter_swap(First, --Last);
		}
	}

	template<class BidIt> inline
	void reverse(BidIt First, BidIt Last)
	{
		detail::Reverse( First, Last,  typename i3::iterator_traits<BidIt>::iterator_category() );
	}
}