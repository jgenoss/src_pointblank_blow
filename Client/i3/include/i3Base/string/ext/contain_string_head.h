#pragma once

#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_literal.h"
#include "../compare/is_iequal.h"

// 이 경우엔 해당 문자열 포함여부를 찾지말고, 그냥 오른쪽 인수 크기만큼 비교해서 처리한다..

namespace i3
{
	template<class Range1, class Range2> inline
	bool contain_string_head(const Range1& str_src, const Range2& str_find)
	{
		typedef typename i3::get_iterator_range_type<const Range1>::type	range1_type;
		typedef typename i3::get_iterator_range_type<const Range2>::type	range2_type;
		range1_type r1 = as_literal(str_src);
		range2_type r2 = as_literal(str_find);
		if (r2.size() < r1.size() ) r1.advance_end( -std::ptrdiff_t(r1.size() - r2.size() ) );
		return i3::range::equal(r1, r2);	
	}
	
	template<class Range1, class Range2> inline
	bool icontain_string_head(const Range1& str_src, const Range2& str_find)
	{
		typedef typename i3::get_iterator_range_type<const Range1>::type	range1_type;
		typedef typename i3::get_iterator_range_type<const Range2>::type	range2_type;
		range1_type r1 = as_literal(str_src);
		range2_type r2 = as_literal(str_find);
		if (r2.size() < r1.size() ) r1.advance_end( -std::ptrdiff_t(r1.size() - r2.size() ) );
		return i3::range::equal(r1, r2, is_iequal());	
	}

}