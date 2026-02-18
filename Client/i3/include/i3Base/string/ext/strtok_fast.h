#pragma once

//
// 최대한 기존함수와 모양 비슷하게..
// 테스트해보니...구현이 큰 iterator어댑터의 경우 퍼포먼스에 나쁜 영향이 갈것으로 판단이 되고, 
// iterator어댑터를 최대한 적게 쓰는 구현을 따로 만드는게 좋을 것 같다..
//

#include "../algorithm/split_fast.h"
#include "detail/strtok_delimit_functor.h"

namespace i3
{

	template<class DestRange, class SrcRange, class DelimitRange> inline
	i3::vector<DestRange>& strtok_fast( i3::vector<DestRange>& Result, const SrcRange& Input, const DelimitRange& delimit, bool token_compress /*= false*/)
	{
		return i3::split_fast(Result, i3::as_literal(Input), detail::make_strtok_delimit_functor(i3::as_literal(delimit)), token_compress);
	}

	template<class DestRange, class SrcRange> inline 
	i3::vector<DestRange>& strtok_fast( i3::vector<DestRange>& Result, const SrcRange& Input, 
		typename i3::range_value<SrcRange>::type delimit_c, bool token_compress /*= false*/ )
	{
		return i3::split_fast(Result, i3::as_literal(Input), detail::make_strtok_delimit_c_functor(delimit_c), token_compress);
	}

}