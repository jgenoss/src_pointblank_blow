#pragma once

#include "../adaptor/generic_inserter.h"
#include "../../type_traits/has_iterators_erase_mem_fun.h"
#include "../../algorithm/copy_if.h"
#include "../../svector.h"

namespace i3
{
	namespace range
	{
		template<class Range, class OutIt, class Pred1> inline
		typename iterator_return<OutIt>::type copy_if( const Range& r, OutIt res, Pred1 pr)
		{
			return i3::copy_if( i3::begin(r), i3::end(r), res, pr);
		}
		
		template<class Range, class Seq, class Pred1> inline
		typename non_iterator_return<Seq>::type copy_if( const Range& r, Seq& seq, Pred1 pr)
		{
			// inserter면 충분치않을까..
			Seq tmp;
			i3::copy_if( i3::begin(r), i3::end(r), i3::generic_inserter(tmp), pr);		//
			i3::copy_or_swap(seq, tmp);
			return seq;
		}
		
		//
		// inplace 버전..컨테이너를 직접 줄이던가...카피/스웝을 사용해야함..
		// 이걸처리하기 위해서 erase함수가 필요함...
		// 우선 erase가 존재하는 것으로 가정한다..(없으면 어떡하나...)
		//

		namespace detail
		{
			template<class Seq, class Pred1, bool has_erase_function>
			struct copy_if_impl;

			template<class Seq, class Pred1>
			struct copy_if_impl<Seq, Pred1, true>
			{
				static inline Seq& call(Seq& s, Pred1 pr)
				{
					s.erase( i3::copy_if( i3::begin(s), i3::end(s), pr), s.end() );
					return s;
				}
			};

			//
			// erase 미지원 컨테이너의 경우, 반복자 수정 자체가 안되는 경우가 많아서..
			// 반복자를 수정하는 알고리즘 함수를 쓸수가 없다.
			// 
			template<class Seq, class Pred1>
			struct copy_if_impl<Seq, Pred1, false>
			{
				static inline Seq& call(Seq& s, Pred1 pr)
				{
					typedef typename i3::range_value<Seq>::type			value_type;
				
					i3::svector<value_type> tmp_svec(i3::begin(s), i3::end(s) );	// allocator도 필요할수 있지만, 우선 이렇게만 처리..
					Seq tmp(  tmp_svec.begin(), i3::copy_if( tmp_svec.begin(), tmp_svec.end(), pr) );
					i3::copy_or_swap(s, tmp);
					return s;
				}
			};

		}

		template<class Seq, class Pred1> inline
		Seq&	copy_if( Seq& s, Pred1 pr)
		{
			return detail::copy_if_impl<Seq, Pred1, 
				i3::has_iterators_erase_mem_fun<Seq>::value>::call(s, pr);
		}
	}
}

namespace i3range = i3::range;