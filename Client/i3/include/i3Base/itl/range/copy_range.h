#pragma once

#include "detail/enable_if_iterator.h"
#include "iterator_range.h"
#include "../utility/copy_or_swap.h"
#include "literal_begin.h"
#include "literal_end.h"

namespace i3
{
	//
	// Range는 값을 보관하는게 아닌, 기존컨테이너의 특정 구간만 알려주는 pair<IT,IT>에 불과하다..
	// 
	// 이것을 따로 값으로 보관하기 위해서는 원래의 컨테이너 타입으로 생성해서 넘겨야 할 것이다.
	//

	// 
	template<class Seq, class Range> inline 
	Seq copy_range( const Range& r)
	{
		return Seq(begin(r), end(r));
	}
	
	//
	// 출력인수용 함수도 있는게 좋을 것이다..
	//
	template<class Seq, class Range> inline 
	Seq& copy_range(Seq& out, const Range& r)
	{
		Seq tmp( begin(r), end(r) );
		i3::copy_or_swap(out, tmp);
		return out;
	}
	
	template<class,class,class,class>
	class i3::stack_basic_string;
	template<class,class,class>
	class i3::basic_string;

	template<class S, class C, class T, class A, class Range> inline
	i3::stack_basic_string<S,C,T,A>& copy_range(i3::stack_basic_string<S,C,T,A>& out, const Range& r)
	{
		return out.assign(begin(r), end(r));
	}

	template<class C, class T, class A, class Range> inline
	i3::basic_string<C,T,A>& copy_range(i3::basic_string<C,T,A>& out, const Range& r)
	{
		return out.assign(begin(r), end(r));
	}

	template<class Seq> inline
	Seq& copy_range(Seq& out, const Seq& r)
	{
		return out = r;					// 같은 타입의 경우 단순이 대입으로만 끝낸다..
	}

	template<class Seq>
	struct copy_range_functor : public i3::unary_function< iterator_range< typename Seq::const_iterator>, Seq>
	{
		Seq operator()( const iterator_range< typename Seq::const_iterator>& r) const 
		{
			return copy_range<Seq>(r);
		}
	};
	
	template<class Seq, class Range> inline
	typename i3::disable_if_iterator<Seq>::type generic_copy_range(Seq& out, const Range& r)
	{
		i3::copy_range(out, r);
	}
	
	template<class It, class Range> inline
	typename i3::enable_if_iterator<It>::type generic_copy_range(It out, const Range& r)
	{
		out = i3::copy(literal_begin(r), literal_end(r), out);
		*out = typename i3::iterator_value<It>::type();
	}

	template<class Seq, class InIt> inline
	typename i3::disable_if_iterator<Seq>::type generic_copy_range(Seq& out, InIt f, InIt l)
	{
		Seq tmp(f, l);
		i3::copy_or_swap(out, tmp);
	}

	// i3string, 스택스트링의 경우만 일단 오버로딩처리해줌..
	template<class S, class C, class T, class A, class InIt> inline
	void generic_copy_range(i3::stack_basic_string<S,C,T,A>& out, InIt f, InIt l)
	{
		out.assign(f,l);
	}

	template<class C, class T, class A, class InIt> inline
	void generic_copy_range(i3::basic_string<C,T,A>& out, InIt f, InIt l)
	{
		out.assign(f,l);
	}

	template<class It, class InIt> inline
	typename i3::enable_if_iterator<It>::type generic_copy_range(It out, InIt f, InIt l)
	{
		out = i3::copy(f, l, out);
		*out = typename i3::iterator_value<It>::type();
	}
	
}