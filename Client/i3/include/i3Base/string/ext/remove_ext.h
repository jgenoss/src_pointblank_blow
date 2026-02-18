#pragma once

// 구버전 호환을 위한 함수..
// 확장자 제거..
//
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/reverse_iterator.h"
#include "../../itl/range/iterator_range.h"
#include "../../itl/range/algorithm/find.h"
#include "../../itl/range/generic_erase_iter_end.h"
#include "../../itl/range/rbegin.h"
#include "../../itl/range/rend.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	template<class Lit> inline 
	typename i3::enable_if_iterator<Lit>::type
	remove_ext( Lit lit)
	{
		
		i3::iterator_range< Lit > rng = i3::as_literal(lit);

		typedef i3::reverse_iterator<Lit> rev_iterator;

		i3::iterator_range< rev_iterator > rev_rng = i3::make_iterator_range( i3::rbegin(rng), i3::rend(rng) );
	
		rev_iterator rev_it = i3::range::find(rev_rng, '.');

		typedef typename i3::iterator_value<Lit>::type	value_type;
		if ( rev_it != rev_rng.end() )		*rev_it = value_type();
	}
	
	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type
	remove_ext( Seq& s)
	{
		typedef typename i3::range_reverse_iterator<Seq>::type rev_iterator;
		typedef typename i3::range_iterator<Seq>::type			iterator;

		i3::iterator_range<rev_iterator> rev_rng = i3::make_iterator_range( i3::rbegin(s), i3::rend(s) );
		rev_iterator rev_it = i3::range::find(rev_rng, '.');

		if ( rev_it != rev_rng.end() )
		{
			iterator pos = (++rev_it).base();
			i3::generic_erase_iter_end(s, pos);
		}
	}
	
	
	
}