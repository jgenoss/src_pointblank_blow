#pragma once

#include "detail/memchr_trait.h"

// reverse_iterator + find_first_of + not1 조합을 써도 되지만..직관적이지 않아서..
// 별도 구현해둔다..

namespace i3
{
	template<class BidIt1, class FwdIt2> inline
	typename i3::disable_if<detail::is_memchr_wmemchr_enable_it_it<BidIt1, FwdIt2>,
	BidIt1>::type find_last_not_of(BidIt1 f1, BidIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		if ( f1 == l1 )
			return l1;				// 못찾으면 어쨋거나 l1으로 끝낸다..

		BidIt1 it = l1;

		while ( --it  )
		{
			for (FwdIt2 mid2 = f2; ; ++mid2)
			{
				if (mid2 == l2)
					return it;
				if (*it == *mid2) 
					break; 
			}

			if ( f1 == it )
				break;
		}

		return l1;
	}

	template<class BidIt1, class FwdIt2> inline
	typename i3::enable_if<detail::is_memchr_enable_it_it<BidIt1, FwdIt2>,
	BidIt1>::type find_last_not_of(BidIt1 f1, BidIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		if ( f1 == l1 )
			return l1;				// 못찾으면 어쨋거나 l1으로 끝낸다..

		BidIt1 it = l1;

		while ( --it  )
		{
			if (::memchr(f2, int(*it), l2 - f2 ) == 0 )
				return it;

			if ( f1 == it )
				break;
		}

		return l1;
	}

	template<class BidIt1, class FwdIt2> inline
	typename i3::enable_if<detail::is_wmemchr_enable_it_it<BidIt1, FwdIt2>,
	BidIt1>::type find_last_not_of(BidIt1 f1, BidIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		if ( f1 == l1 )
			return l1;				// 못찾으면 어쨋거나 l1으로 끝낸다..

		BidIt1 it = l1;

		while ( --it  )
		{
			if (::wmemchr((const wchar_t*)f2, wchar_t(*it), l2 - f2 ) == 0 )
				return it;

			if ( f1 == it )
				break;
		}

		return l1;
	}


	template<class BidIt1, class FwdIt2, class Pred2> inline
	BidIt1 find_last_not_of(BidIt1 f1, BidIt1 l1, FwdIt2 f2, FwdIt2 l2, Pred2 pr)
	{
		if ( f1 == l1 )
			return l1;				// 못찾으면 어쨋거나 l1으로 끝낸다..

		BidIt1 it = l1;

		while ( --it  )
		{
			for (FwdIt2 mid2 = f2; ; ++mid2)
			{
				if (mid2 == l2)
					return it;
				if ( pr(*it, *mid2) )
					break; 
			}

			if ( f1 == it )
				break;
		}

		return l1;
	}

}