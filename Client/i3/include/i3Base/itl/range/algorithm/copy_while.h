#pragma once

#include "../adaptor/generic_inserter.h"
#include "../../type_traits/has_iterators_erase_mem_fun.h"
#include "../../algorithm/copy_while.h"

namespace i3
{
	namespace range
	{
		template<class Range, class OutIt, class Pred1> inline
		typename iterator_return<OutIt>::type copy_while( const Range& r, OutIt res, Pred1 pr)
		{
			return i3::copy_while( i3::begin(r), i3::end(r), res, pr);
		}

		template<class Range, class Seq, class Pred1> inline
		typename non_iterator_return<Seq>::type copy_while( const Range& r, Seq& seq, Pred1 pr)
		{
			// inserter면 충분치않을까..
			Seq tmp;
			i3::copy_while( i3::begin(r), i3::end(r), i3::generic_inserter(tmp), pr);		//
			i3::copy_or_swap(seq, tmp);
			return seq;
		}

		//
		// inplace 버전..컨테이너를 직접 줄이던가...카피/스웝을 사용해야함..
		// 이걸처리하기 위해서 erase함수가 필요함...
		// erase 함수가 없다면, 카피or스웝으로 굴린다..
		// 아래 루틴은 generic_erase에서 재활용 가능하므로, 루틴이 추가되면
		//  그 루틴으로 구현을 옮겨야한다..
		// 
		namespace detail
		{
			template<class Seq, class Pred1, bool has_erase_function>
			struct copy_while_impl;

			template<class Seq, class Pred1>
			struct copy_while_impl<Seq, Pred1, true>
			{
				static inline Seq& call(Seq& s, Pred1 pr)
				{
					s.erase( i3::copy_while( i3::begin(s), i3::end(s), pr), s.end() );
					return s;
				}
			};
			template<class Seq, class Pred1>
			struct copy_while_impl<Seq, Pred1, false>
			{
				static inline Seq& call(Seq& s, Pred1 pr)
				{
					typedef typename i3::range_value<Seq>::type			value_type;
					i3::svector<value_type> tmp_svec(i3::begin(s), i3::end(s) );	// allocator도 필요할수 있지만, 우선 이렇게만 처리..
					Seq tmp(  tmp_svec.begin(), i3::copy_while( tmp_svec.begin(), tmp_svec.end(), pr) );
					i3::copy_or_swap(s, tmp);
					return s;
				}
			};
			
		}

		template<class Seq, class Pred1> inline
		Seq&	copy_while( Seq& s, Pred1 pr)
		{
			return detail::copy_while_impl<Seq, Pred1, 
				i3::has_iterators_erase_mem_fun<Seq>::value>::call(s, pr);
		}
	}
}

namespace i3range = i3::range;