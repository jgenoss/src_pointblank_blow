#pragma once

#include "../../itl/range/algorithm/remove.h"
#include "../../itl/range/value_type.h"
#include "../../itl/iterator/iterator_traits.h"

// 함수의 첫 인수 인수추론 문제로 함수가 하나로 되지는 않는다..

namespace i3
{
	template<class Literal> inline typename i3::enable_if_iterator<Literal>::type 
	remove_all_char(Literal ctn, typename i3::iterator_value<Literal>::type c)
	{
		i3::range::remove(ctn, c);
	}
	
	template<class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_all_char(Seq& s, typename i3::range_value<Seq>::type c)
	{
		i3::range::remove(s, c);
	}
	
	
}