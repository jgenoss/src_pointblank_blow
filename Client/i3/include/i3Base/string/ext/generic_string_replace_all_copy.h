#pragma once

#include "../algorithm/replace_all.h"
#include "../stack_string.h"

//
// i3::replace_all_copy의 out컨테이너가 const반복자만 가진 i3::rc_string의 경우엔 실행이 안되므로,
// 대체컨테이너로 먼저 처리후 복사하는 함수가 별도로 필요함..
//

namespace i3
{
	template<class OutLit, class InputRng, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<OutLit>::type
	generic_string_replace_all_copy(OutLit out, const InputRng& in, const BeforeRng& before, const AfterRng& after)
	{
		typedef typename i3::range_value<OutLit>::type		Ch;
		OutLit end = i3::replace_all_copy(out, i3::as_literal(in), before, after);
		*end = 0;
	}
	
	template<class OutSeq, class InputRng, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<OutSeq>::type
	generic_string_replace_all_copy(OutSeq& out, const InputRng& in, const BeforeRng& before, const AfterRng& after)
	{
		out = OutSeq(literal_begin(in), literal_end(in) );
		i3::replace_all(out, before, after);		
	}
		
	template<class OutLit, class InputRng, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<OutLit>::type
	generic_string_ireplace_all_copy(OutLit out, const InputRng& in, const BeforeRng& before, const AfterRng& after)
	{
		typedef typename i3::range_value<OutLit>::type		Ch;
		OutLit end = i3::ireplace_all_copy(out, i3::as_literal(in), before, after);
		*end = 0;
	}
	
	template<class OutSeq, class InputRng, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<OutSeq>::type
	generic_string_ireplace_all_copy(OutSeq& out, const InputRng& in, const BeforeRng& before, const AfterRng& after)
	{
		out = OutSeq(literal_begin(in), literal_end(in) );
		i3::ireplace_all(out, before, after);		
	}
		
}
