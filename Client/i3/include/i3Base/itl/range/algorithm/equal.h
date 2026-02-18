#pragma once

#include "../begin.h"
#include "../end.h"

// 일반 알고리즘과 차이점은 레인지(컨테이너)의 경우 크기 점검이 추가된다는 점..

namespace i3
{
	namespace range_detail
	{
		template<class It1, class It2, class Cat1, class Cat2> inline 
		bool equal_impl(It1 f1, It1 l1, It2 f2, It2 l2, Cat1, Cat2)
		{
			for ( ; f1 != l1 ; ++f1, ++f2)
			{
				if ( f2 == l2 )
					return false;
				if ( !(*f1 == *f2) )
					return false;
			}
			return (f2 == l2);
		}

		template<class It1, class It2, class Fn2, class Cat1, class Cat2> inline 
		bool equal_impl(It1 f1, It1 l1, It2 f2, It2 l2, Fn2 fn2, Cat1, Cat2)
		{
			for ( ; f1 != l1 ; ++f1, ++f2)
			{
				if ( f2 == l2 )
					return false;
				if ( !fn2(*f1, *f2) )
					return false;
			}
			return (f2 == l2);
		}
		
		template<class It1, class It2> inline 
		bool equal_impl(It1 f1, It1 l1, It2 f2, It2 l2, i3::random_access_iterator_tag, i3::random_access_iterator_tag)
		{
			return ( (l1 - f1) == (l2 - f2) ) && i3::equal(f1, l1, f2);
		}

		template<class It1, class It2, class Fn2> inline 
		bool equal_impl(It1 f1, It1 l1, It2 f2, It2 l2, Fn2 fn2, i3::random_access_iterator_tag, i3::random_access_iterator_tag)
		{
			return ( (l1 - f1) == (l2 - f2) ) && i3::equal(f1, l1, f2, fn2);
		}
		
		template<class It1, class It2> inline
		bool equal(It1 f1, It1 l1, It2 f2, It2 l2)
		{
			typename i3::iterator_traits<It1>::iterator_category cat1;
			typename i3::iterator_traits<It2>::iterator_category cat2;
			return equal_impl(f1, l1, f2, l2, cat1, cat2);
		}

		template<class It1, class It2, class Fn2> inline
		bool equal(It1 f1, It1 l1, It2 f2, It2 l2, Fn2 fn2)
		{
			typename i3::iterator_traits<It1>::iterator_category cat1;
			typename i3::iterator_traits<It2>::iterator_category cat2;
			return equal_impl(f1, l1, f2, l2, fn2, cat1, cat2);
		}
	}

	namespace range
	{
		
		template<class SinglePassRange1, class SinglePassRange2> inline 
		bool equal( const SinglePassRange1& r1, const SinglePassRange2& r2)
		{
			return range_detail::equal(i3::begin(r1), i3::end(r1), i3::begin(r2), i3::end(r2) );
		}
		
		template<class SinglePassRange1, class SinglePassRange2, class Fn2> inline 
		bool equal( const SinglePassRange1& r1, const SinglePassRange2& r2, Fn2 fn2)
		{
			return range_detail::equal(i3::begin(r1), i3::end(r1), i3::begin(r2), i3::end(r2), fn2);
		}
	}

}

namespace i3range = i3::range;