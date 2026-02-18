#pragma once

// NormalizePath함수의 일반형 구현이다.
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/generic_erase_iter_end.h"
#include "../../itl/range/empty.h"
#include "make_generic_literal.h"

#include "generic_find_first_not_of.h"

namespace i3
{

	template<class Lit> 
	typename enable_if_iterator<Lit>::type
	remove_end_slash( Lit inout )
	{
		typedef typename iterator_value<Lit>::type	value_type;
		size_t num = i3::char_traits<value_type>::length(inout);
		if (!num) return;
		Lit  last_char_iter = inout + num - 1;
		while (*last_char_iter == value_type('\\') ||
			   *last_char_iter == value_type('/') )
		{
			*last_char_iter = value_type();
			--last_char_iter;
		}
	}	

	template<class Seq> 
	typename disable_if_iterator<Seq>::type
	remove_end_slash( Seq& inout )
	{
		if ( i3::empty(inout)) return;

		typedef typename i3::range_reverse_iterator<Seq>::type rev_iterator;
		typedef typename i3::range_iterator<Seq>::type	iterator;
		typedef typename range_value<Seq>::type			Ch;
		rev_iterator rev_it = generic_find_first_not_of( i3::make_reverse_iterator_range(inout), i3::make_generic_literal(Ch ,"\\/") );
		iterator it = rev_it.base();

		if ( it == begin(inout) ) return;

		i3::generic_erase_iter_end(inout, it);
	}	

}