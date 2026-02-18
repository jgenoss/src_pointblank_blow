#pragma once

//
//   i3String::CompareNoCase함수 일반화..  (i3String::Compare는 대소문자를 가리지 않는 것에 유의)
//   ( 개인적으로는 Compare함수보다는 equal/less쪽을 추천한다..)
//	 -1,0,1 로 구분한다.

#include "../../itl/range/algorithm/compare.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	template<class Rng1, class Rng2> inline
	int	generic_compare( const Rng1& lhs, const Rng2& rhs )	
	{
		return i3::range::compare(i3::as_literal(lhs), i3::as_literal(rhs));
	}
}

