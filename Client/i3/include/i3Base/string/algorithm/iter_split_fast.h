#pragma once

//
// 이터레이터 어댑터 사용을 자제할 것..제약이 심하게 들어가게 된다..
// 1. 문자열을 담는 컨테이너가 확정되어야 한다. (vector로 고정)
//
#include "../../itl/range/as_literal.h"

namespace i3
{
	template<class DestRange, class SrcRange, class Finder> inline
	i3::vector<DestRange>& iter_split_fast( i3::vector<DestRange>& Result, const SrcRange& Input, Finder Fnd)
	{
		Result.clear();

		typedef typename range_iterator<SrcRange>::type		input_iterator_type;
		typedef iterator_range<input_iterator_type>			iterator_range_type;

		iterator_range_type lit_input( as_literal(Input) );
		
		input_iterator_type InputBegin = i3::begin(lit_input);
		input_iterator_type InputEnd = i3::end(lit_input);
				
		for ( ; InputBegin != InputEnd ; )
		{
			iterator_range_type find_res = Fnd(InputBegin, InputEnd);		// 하나씩만 찾게 된다..
			Result.push_back( DestRange(InputBegin, find_res.begin() ) );
			InputBegin = find_res.end();
		}
		
		return Result;
	}
	
}