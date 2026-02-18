#pragma once

#include "erase_string.h"

#include "detail/remove_blank_detail.h"
#include "../../itl/range/algorithm/find_if.h"
#include "../../itl/iterator/iterator_traits.h"

namespace i3
{
	//
	// 구 i3Engine 함수와 호환성을 맞춘 함수..
	//



	// 컨테이너 쪽..
	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type remove_blank_from_head( Seq& s)
	{
	
		typedef typename i3::range_iterator<Seq>::type iterator;
		iterator it = range::find_if(s, detail::find_non_blank_pos_functor());	
		if ( it == i3::end(s) ) 
			return;		
		
		i3::erase_string(s, 0, std::size_t(i3::distance( i3::begin(s), it )) );
	}
	
	template<class It> inline
	typename i3::enable_if_iterator<It>::type remove_blank_from_head(It it)
	{
		// 특별이 알고리즘을 동원하지 말고...그냥 찾아본다...

		typedef typename i3::iterator_value<It>::type value_type;

		It  pos_it = it;
		detail::find_non_blank_pos_functor fn;

		for ( ; *pos_it != value_type() && !fn(*pos_it) ; ++pos_it) {}
		
		if ( !(*pos_it) ) 
			return;
		
		i3::erase_string(it, 0, std::size_t(i3::distance( it, pos_it )) );
	}

	
}