#pragma once

//
// 본래 불필요해보이나, 호환성 문제때문에 Range 알고리즘으로 추가해둔다..
// 오버로딩함수는 비교함수객체 대신에 변형함수객체를 넣는다... (비교함수라면 인수가 2개가 필요하고, icompare구현에는 오히려 부담이 된다..)
//
//

#include "../begin.h"
#include "../end.h"
#include <wchar.h>
#include "../../algorithm/detail/memcmp_traits.h"

namespace i3
{
	namespace range_detail
	{
		
		template<class It1, class It2, class TransFn1>  
		int compare(It1 f1, It1 l1, It2 f2, It2 l2, TransFn1 fn)
		{
			typedef typename i3::iterator_traits<It1>::value_type		value1_type;
			typedef typename i3::iterator_traits<It2>::value_type		value2_type;

			value1_type v1;
			value2_type v2;

			for ( ; f1 != l1 ; ++f1, ++f2)
			{
				if ( f2 == l2 )
					return 1;
				
				v1 = fn(*f1);	v2 = fn(*f2);

				if ( !(v1 == v2) )
				{
					return v1 < v2 ? -1 : 1;
				}
			}

			return (f2 == l2) ? 0 : -1;			// f2 != l2라면 우측값이 더 큰 케이스다..
		}
		


		template<class It1, class It2>  
		typename i3::disable_if<detail::is_memcmp_wmemcmp_enable<It1, It2>,
		int>::type compare_impl(It1 f1, It1 l1, It2 f2, It2 l2)
		{
			for ( ; f1 != l1 ; ++f1, ++f2)
			{
				if ( f2 == l2 )			// 좌측값이 더 큰 케이스..
					return 1;
				if ( !(*f1 == *f2) )
				{
					return ( *f1 < *f2 ) ? -1 : 1;
				}
			}

			return (f2 == l2) ? 0 : -1;			// f2 != l2라면 우측값이 더 큰 케이스다..
		}

		template<class InIt1, class InIt2> inline 
		typename i3::enable_if<detail::is_memcmp_enable<InIt1, InIt2>,
		int>::type compare_impl( InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
		{	
			ptrdiff_t Num1 = l1 - f1;
			ptrdiff_t Num2 = l2 - f2;
			int Ans = ::memcmp(f1, f2, Num1 < Num2 ? Num1 : Num2);
			return (Ans != 0 ) ? Ans : Num1 - Num2;
		}

		template<class InIt1, class InIt2> inline 
		typename i3::enable_if<detail::is_wmemcmp_enable<InIt1, InIt2>,
		int>::type compare_impl(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
		{	
			ptrdiff_t Num1 = l1 - f1;
			ptrdiff_t Num2 = l2 - f2;
			int Ans = ::wmemcmp((const wchar_t*)f1, (const wchar_t*)f2, Num1 < Num2 ? Num1 : Num2);
			return (Ans != 0 ) ? Ans : Num1 - Num2;
		}
		
		template<class It1, class It2> inline
		int compare(It1 f1, It1 l1, It2 f2, It2 l2)
		{
			return compare_impl(f1, l1, f2, l2);
		}

	}

	namespace range
	{
		template<class SinglePassRange1, class SinglePassRange2> inline 
		int compare( const SinglePassRange1& r1, const SinglePassRange2& r2)
		{
			return range_detail::compare(i3::begin(r1), i3::end(r1), i3::begin(r2), i3::end(r2) );
		}
		
		template<class SinglePassRange1, class SinglePassRange2, class TransFn1> inline 
		int compare( const SinglePassRange1& r1, const SinglePassRange2& r2, TransFn1 fn)
		{
			return range_detail::compare(i3::begin(r1), i3::end(r1), i3::begin(r2), i3::end(r2), fn);
		}
		
	}

}

namespace i3range = i3::range;
