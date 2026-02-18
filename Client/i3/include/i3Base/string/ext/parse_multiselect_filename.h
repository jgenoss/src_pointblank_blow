#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/value_type.h"

namespace i3
{
	// 기존에 있던것 호환성을 위해 추가..
	// 문자열 안에 널값이 군데군데 섞여있고, 해당 널값마다 문자열 분리하는 함수이다.
	
	// 문자 배열뿐 아니라...string container에서 중간에 널값이 가능한 것을 이용해야함.
	
	template<class Lit, class OutSeq> typename enable_if_iterator<Lit>::type
	parse_multiselect_filename( Lit lit, i3::vector<OutSeq>& inout)
	{
		Lit lit_tmp = lit;

		typedef typename get_iterator_range_type<Lit>::type		iter_range_type;
		iter_range_type rng = i3::as_literal(lit_tmp);
		while ( !rng.empty() )
		{
			inout.push_back( OutSeq( rng.begin(), rng.end() ) );

			i3::advance( lit_tmp, rng.size() + 1 );		// 하나 더 증가..

			rng = i3::as_literal(lit_tmp);
		}
	}

	template<class Seq, class OutSeq> typename disable_if_iterator<Seq>::type
	parse_multiselect_filename( const Seq& seq, i3::vector<OutSeq>& inout)
	{
		typedef typename range_value<const Seq>::type		value_type;
		typedef typename range_iterator<const Seq>::type	iterator;

		iterator it_f = begin(seq);
		iterator it_end = end(seq);
		iterator it_l = i3::find(it_f, it_end, value_type('\0') );
		
		while ( it_l != it_end ) 
		{
			inout.push_back(OutSeq( it_f, it_l) );
			it_f = it_l;	++it_f;
			it_l = i3::find( it_f, it_end, value_type('\0') );
		}
	}
	
	
	
}