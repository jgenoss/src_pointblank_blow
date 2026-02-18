#pragma once

#include "../../itl/range/copy_range.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"

// 카피된 사이즈가 리턴되도록 조절함..

namespace i3
{
	
	template<class Seq, class Range> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, const Range& r)
	{
		typedef typename i3::range_value<Seq>::type		Ch1;
		typedef typename i3::get_iterator_range_type<const Range>::type iter_range_type;
		typedef typename iter_range_type::value_type	Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		i3::copy_range(out, i3::as_literal(r) );
		return out.size();
	}

	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, const Seq& r)
	{
		out = r;
		return out.size();
	}

	template<class It, class Range> inline
	typename i3::enable_if_iterator<It, size_t>::type generic_string_copy(It out, const Range& r)
	{
		typedef typename i3::iterator_value<It>::type	Ch1;
		typedef typename i3::get_iterator_range_type<const Range>::type iter_range_type;
		typedef typename iter_range_type::value_type	Ch2;
		compile_assert_pred( ( i3::is_same<Ch1, Ch2> ) );

		It next = i3::copy(i3::literal_begin(r), i3::literal_end(r), out);
		*next = typename i3::iterator_value<It>::type();
		return i3::distance(out, next);
	}

	template<class Seq, class InIt> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, InIt f, InIt l)
	{
		Seq tmp(f, l);
		i3::copy_or_swap(out, tmp);
		return out.size();
	}

	template<class,class,class,class>
	class i3::stack_basic_string;
	template<class,class,class>
	class i3::basic_string;

	// i3::string과 스택스트링의 경우 오버로딩처리해줌..
	template<class S, class C, class T, class A, class InIt> inline
	size_t generic_string_copy(i3::stack_basic_string<S,C,T,A>& out, InIt f, InIt l)
	{
		out.assign(f,l);
		return out.size();
	}

	template<class C, class T, class A, class InIt> inline
	size_t generic_string_copy(i3::basic_string<C,T,A>& out, InIt f, InIt l)
	{
		out.assign(f,l);
		return out.size();
	}

	template<class It, class InIt> inline
	typename i3::enable_if_iterator<It, size_t>::type generic_string_copy(It out, InIt f, InIt l)
	{
		It next = i3::copy(f, l, out);
		*next = typename i3::iterator_value<It>::type();
		return i3::distance(out, next);
	}

}