#pragma once

//
// i3String::Replace의 대체함수..
//  replace_all을 활용한다.. 리터럴 호환성이 필요해서 좀더 코드가 붙는다.
//	generic_nreplace_all함수도 필요할 것이다..(이건 작업을 좀 미룬다..)
//

#include "../algorithm/replace_all.h"
#include "../stack_string.h"

namespace i3
{
	template<class InOutLit, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<InOutLit>::type
	generic_string_replace_all(InOutLit out, const BeforeRng& before, const AfterRng& after)
	{
		typedef typename i3::range_value<InOutLit>::type			Ch;
		i3::stack_basic_string<mpl::size_t<256>, Ch>  tmp(out);			// 리터럴의 경우 일단 복사가 필요하다. 
		InOutLit end = i3::replace_all_copy(out, tmp, before, after);
		*end = 0;
	}
	
	template<class InOutSeq, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<InOutSeq>::type
	generic_string_replace_all(InOutSeq& out, const BeforeRng& before, const AfterRng& after)
	{
		i3::replace_all(out, before, after);		
	}
		
	template<class InOutLit, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<InOutLit>::type
	generic_string_ireplace_all(InOutLit out, const BeforeRng& before, const AfterRng& after)
	{
		typedef typename i3::range_value<InOutLit>::type			Ch;
		i3::stack_basic_string<mpl::size_t<256>, Ch>  tmp(out);			// 리터럴의 경우 일단 복사가 필요하다. 
		InOutLit end = i3::ireplace_all_copy(out, tmp, before, after);
		*end = 0;
	}
	
	template<class InOutSeq, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<InOutSeq>::type
	generic_string_ireplace_all(InOutSeq& out, const BeforeRng& before, const AfterRng& after)
	{
		i3::ireplace_all(out, before, after);		
	}
	
	
}