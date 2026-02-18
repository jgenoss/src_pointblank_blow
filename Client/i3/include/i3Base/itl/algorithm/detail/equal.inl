
//
#include "memcmp_traits.h"
#include "../../enable_if.h"

namespace i3
{
	namespace detail
	{
				
		template<class InIt1, class InIt2> inline
		typename i3::disable_if<detail::is_memcmp_wmemcmp_enable<InIt1, InIt2>,
		bool>::type equal(InIt1 First1, InIt1 Last1, InIt2 First2 )
		{	// compare [_First1, _Last1) to [First2, ...)
			for (; First1 != Last1; ++First1, ++First2)
				if (!(*First1 == *First2))
					return false;
			return true;
		}

		template<class InIt1, class InIt2>  inline 
		typename i3::enable_if<detail::is_memcmp_enable<InIt1, InIt2>,		
		bool>::type equal(InIt1 First1,	InIt1 Last1, InIt2 First2)
		{	// compare [_First1, _Last1) to [First2, ...), for chars
			return (::memcmp(First1, First2, Last1 - First1) == 0);
		}

		template<class InIt1, class InIt2>  inline 
		typename i3::enable_if<detail::is_wmemcmp_enable<InIt1, InIt2>,		
		bool>::type equal(InIt1 First1,	InIt1 Last1, InIt2 First2)
		{
			return (::wmemcmp((const wchar_t*)First1, (const wchar_t*)First2,  Last1 - First1) == 0);
		}
		
	}

	template<class InIt1, class InIt2> inline
	bool equal(InIt1 f1, InIt1 l1, InIt2 f2)
	{
		return detail::equal(f1, l1, f2 );
	}

	template<class InIt1, class InIt2, class Pred2> inline
	bool equal(InIt1 f1, InIt1 l1, InIt2 f2, Pred2 pr)
	{
		for (; f1 != l1; ++f1, ++f2)
		{
			if (!pr(*f1, *f2)) return false;
		}
		return true;
	}

}
