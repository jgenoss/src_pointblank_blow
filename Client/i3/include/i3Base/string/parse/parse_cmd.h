#pragma once

//
// 1개의 command에 대해 parsing하는 루틴이 필요함..
//

#include "../../itl/range/as_literal.h"
#include "../compare/is_iequal.h"
#include "find_non_whitespace.h"

namespace i3
{
	namespace detail
	{
		//
		// cmd문자열을 통과했다면, 명령으로 받아들인다..
		// Fn2의 인수는 인풋문자열서두와 cmd문자열이 일치할때, 인풋 커맨드 다음 포인터와 끝포인터가 된다..
		//

		template<class IterRng, class OutIt>	
		bool parse_cmd_impl(const IterRng& input_rng, const IterRng& str_cmd, OutIt& arg_start, OutIt& arg_end)
		{
			// 유효문자열 이전의 공백을 먼저 우선적으로 제거한다. (공백문자열은 함수인수로 받지 않고, 고정비교한다...)
			typedef typename IterRng::iterator						iterator;
			typedef typename IterRng::value_type					Ch;

			iterator start_pos = i3::find_non_whitespace(input_rng);
			iterator end_pos   = input_rng.end();

			if ( end_pos - start_pos < str_cmd.size() )			// 명령 문자열보다 작게 되면 그냥 리턴...
				return false;

			if ( i3::equal( str_cmd.begin(), str_cmd.end(), start_pos, i3::is_iequal() ) == false )		// 더 짧을수 있는 것을 첫인수로, 길수도 있는것이 뒷 인수로 간다..
				return false;
			
			i3::advance( start_pos, str_cmd.size() );		// 커맨드비교가 끝났으면 start_pos를 증가시켜도 좋음..

			arg_start = start_pos;
			arg_end   = end_pos;
			
			return true;
		}
	}

	// as_literal처리를 여기서 하고 detail로 구현을 옮긴다..
	template<class InputRng, class CmdRng, class OutIt>	inline
	bool	parse_cmd(const InputRng& input_rng, const CmdRng& str_cmd, OutIt& arg_start, OutIt& arg_end)
	{
		return detail::parse_cmd_impl(i3::as_literal(input_rng), i3::as_literal(str_cmd), arg_start, arg_end);
	}
	


}