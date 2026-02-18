
#include <wchar.h>
#include "memcmp_traits.h"

namespace i3
{
	namespace detail
	{
		
		template<class InIt1, class InIt2> inline
		typename i3::disable_if<detail::is_memcmp_wmemcmp_enable<InIt1, InIt2>,
		bool>::type lexicographical_compare_impl(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
		{	
			for (; f1 != l1 && f2 != l2; ++f1, ++f2)
			{
				if ( *f1 < *f2 ) return true;
				else if (*f2 < *f1) return false;
			}
			return (f1 == l1 && f2 != l2);
		}
		
		template<class InIt1, class InIt2> inline 
		typename i3::enable_if<detail::is_memcmp_enable<InIt1, InIt2>,
		bool>::type lexicographical_compare_impl(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
		{	
			ptrdiff_t Num1 = l1 - f1;
			ptrdiff_t Num2 = l2 - f2;
			int Ans = ::memcmp(f1, f2, Num1 < Num2 ? Num1 : Num2);
			return (Ans < 0 || Ans == 0 && Num1 < Num2);
		}

		template<class InIt1, class InIt2> inline 
		typename i3::enable_if<detail::is_wmemcmp_enable<InIt1, InIt2>,
		bool>::type lexicographical_compare_impl(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
		{	
			ptrdiff_t Num1 = l1 - f1;
			ptrdiff_t Num2 = l2 - f2;
			int Ans = ::wmemcmp((const wchar_t*)f1, (const wchar_t*)f2, Num1 < Num2 ? Num1 : Num2);
			return (Ans < 0 || Ans == 0 && Num1 < Num2);
		}

	}


	template<class InIt1, class InIt2> inline
	bool lexicographical_compare(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
	{
		return detail::lexicographical_compare_impl(f1, l1, f2, l2);
	}

	template<class InIt1, class InIt2, class Pred2> inline
	bool lexicographical_compare(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, Pred2 pr)
	{
		for (; f1 != l1 && f2 != l2; ++f1, ++f2)
			if ( pr( *f1, *f2) ) return true;
			else if (pr(*f2, *f1) ) return false;
		return (f1 == l1 && f2 != l2);
	}
	

}