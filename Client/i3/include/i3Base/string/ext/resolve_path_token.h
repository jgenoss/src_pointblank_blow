#pragma once

// i3String::ResolvePathToken함수의 일반화버전 ( 구버전 함수는 비교적 쓰임이 있었음)
#include "strtok_fast.h"
#include "make_generic_literal.h"

namespace i3
{
//  리터럴을 일반화하기가 좀 어려운것 같다..

	template<class InRng, class DestRng> inline
	void	resolve_path_token(const InRng& path, i3::vector<DestRng>& out_list)
	{
		typedef typename i3::range_value<DestRng>::type		Ch;
		i3::strtok_fast(out_list, path, i3::make_generic_literal(Ch, "\\/") );
	}

}
