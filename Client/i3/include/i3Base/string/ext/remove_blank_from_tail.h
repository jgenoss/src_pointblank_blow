#pragma once

#include "erase_string.h"
#include "detail/remove_blank_detail.h"
#include "../../itl/range/rbegin.h"
#include "../../itl/range/rend.h"
#include "../../itl/range/algorithm/find_if.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	// 컨테이너 쪽..
	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type remove_blank_from_tail( Seq& s)
	{
		typedef typename i3::range_reverse_iterator<Seq>::type r_iterator;
		
		i3::iterator_range<r_iterator> r = i3::make_iterator_range(i3::rbegin(s), i3::rend(s) );
		r_iterator rit = range::find_if(r, detail::find_non_blank_pos_functor());	
		if ( rit == i3::end(r) ) 
			return;		
		
		size_t start_pos = i3::distance(i3::begin(s), rit.base() );
		size_t num_erase = i3::generic_string_size(s) - start_pos;

		i3::erase_string(s, start_pos, num_erase);
	}

	template<class It> inline
	typename i3::enable_if_iterator<It>::type remove_blank_from_tail(It it)
	{
		// 이 경우엔 길이값을 미리 알아야 하므로, 그냥 iterator_range를 처음부터 도입.
		typedef typename i3::range_iterator<It>::type  range_iterator;

		i3::iterator_range<range_iterator> r_orig = i3::as_literal(it);
		//
		typedef i3::reverse_iterator<range_iterator> r_iterator;

		i3::iterator_range<r_iterator> r_rev = 
			i3::make_iterator_range(i3::rbegin(r_orig), i3::rend(r_orig) );

		r_iterator rit = range::find_if(r_rev, detail::find_non_blank_pos_functor());	
		if ( rit == i3::end(r_rev) ) 
			return;				

		size_t start_pos = i3::distance(i3::begin(r_orig), rit.base() );
		size_t num_erase = r_orig.size() - start_pos;
	
		i3::erase_string(it, start_pos, num_erase );
	}

}