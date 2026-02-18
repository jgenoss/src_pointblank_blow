#pragma once

#include "detail/memchr_trait.h"

namespace i3
{
	template<class FwdIt1, class FwdIt2> inline
	typename i3::disable_if<detail::is_memchr_wmemchr_enable_it_it<FwdIt1, FwdIt2>,
	FwdIt1>::type find_first_not_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		for (; f1 != l1; ++f1)
		{
			for (FwdIt2 mid2 = f2;  ; ++mid2)
			{
				if ( mid2 == l2 )
					return f1;
				if (*f1 == *mid2)
					break;
			}
		
		}
		return f1;
	}

	template<class FwdIt1, class FwdIt2> inline
	typename i3::enable_if<detail::is_memchr_enable_it_it<FwdIt1, FwdIt2>,
	FwdIt1>::type find_first_not_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		for (; f1 != l1; ++f1)
		{
			if (::memchr(f2, int(*f1), l2 - f2 ) == 0 )
				return f1;
		}
		return f1;
	}

	template<class FwdIt1, class FwdIt2> inline
	typename i3::enable_if<detail::is_wmemchr_enable_it_it<FwdIt1, FwdIt2>,
	FwdIt1>::type find_first_not_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		for (; f1 != l1; ++f1)
		{
			if (::wmemchr((const wchar_t*)f2, wchar_t(*f1), l2 - f2 ) == 0 )
				return f1;
		}
		return f1;
	}

	template<class FwdIt1, class FwdIt2, class Pred2> inline
	FwdIt1 find_first_not_of(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2, Pred2 pr)
	{
		for (; f1 != l1; ++f1)
		{
			for (FwdIt2 mid2 = f2; ; ++mid2)
			{
				if (mid2 == l2)
					return f1;
				if (pr(*f1, *mid2)) 
					break;
			}
		}
		return f1;
	}
	
}
