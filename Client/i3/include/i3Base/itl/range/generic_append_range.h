#pragma once

// 끝에다, insert_iterating 하는 함수인데, 범위 인수를 지정하지 않고 range를 통째로 받는다.

#include "generic_insert_iterators.h"
#include "as_literal.h"

namespace i3
{

	template<class Literal, class InIt> inline 
	typename i3::enable_if_iterator<Literal, size_t>::type 
	generic_append_range(Literal ctn, InIt f, InIt l)
	{
		typedef Literal										output_iterator;
		typedef typename i3::iterator_value<Literal>::type	value_type;

		output_iterator end_it = ctn;
		i3::advance(end_it, i3::char_traits<value_type>::length( &(*ctn) ) );
	
		end_it = i3::copy( f, l, end_it);
		*end_it = value_type();

		return i3::distance(f, l);
	}

	template<class Literal, class Range> inline 
	typename i3::enable_if_iterator<Literal, size_t>::type 
	generic_append_range(Literal ctn, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type iter_rng =  i3::as_literal(rng);
		return generic_append_range(ctn, iter_rng.begin(), iter_rng.end() );
	}

	//
	// insert(iter, iter, iter)가 필요함..없다면, copy_swap으로 대신 처리한다..
	//

	namespace detail
	{
		
		template<class Seq, class InIt, bool append_mem_fun_friendly>
		struct generic_append_range_impl;

		template<class Seq, class InIt>
		struct generic_append_range_impl<Seq, InIt, true>
		{
			
			static inline size_t call(Seq& s, InIt f, InIt l)
			{
				s.append(f, l);
				return i3::distance(f, l);
			}
		};

		template<class Seq, class InIt>
		struct generic_append_range_impl<Seq, InIt, false>
		{
			
			static inline size_t call(Seq& s, InIt f, InIt l)
			{
				// 일반적인 insert를 사용한다..
				i3::generic_insert_iterators(s, end(s), f, l);
				return i3::distance(f, l);
			}
		};

	}

	template<class Seq, class InIt> inline 
		typename i3::disable_if_iterator<Seq,size_t>::type 
		generic_append_range(Seq& s, InIt f, InIt l)
	{
		static const bool append_mem_fun_friendly = mpl::or_< 
			is_template_same< i3::basic_string<mpl_, mpl_, mpl_>, Seq> , 
			is_template_same< i3::stack_basic_string<mpl_, mpl_, mpl_, mpl_>, Seq> >::type::value;
		
		return detail::generic_append_range_impl<Seq, InIt,  
			append_mem_fun_friendly>::call(s, f, l );	
	}

	template<class Seq, class Range> inline 
	typename i3::disable_if_iterator<Seq, size_t>::type 
	generic_append_range(Seq& s, const Range& rng)
	{
		typedef typename get_iterator_range_type<const Range>::type iter_range_type;
		iter_range_type iter_rng =  i3::as_literal(rng);
		
		return generic_append_range(s, iter_rng.begin(), iter_rng.end());
	}


	
	
}
