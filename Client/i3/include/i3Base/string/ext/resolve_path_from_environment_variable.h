#pragma once

#include "get_environment_variable.h"

#include "../../itl/range/generic_clear.h"
//
// i3System::ResolvePath 대체 버전..
//
// 이 버전도 리터럴 출력 버전은 제외시킨다..
// $(XXXX)에서 XXXX를 추출해서 환경값을 얻어와서 변경시켜주면 된다..
//

namespace i3
{
	namespace detail
	{
		template<class ItRng, class OutSeq> inline
		void resolve_path_from_environment_variable_impl(const ItRng& rng, OutSeq& output)
		{
			typedef typename ItRng::iterator	InIt;
			typedef typename ItRng::value_type	Ch;
			typedef mpl::size_t<i3::stack_string::BuffSize>		BuffSizeType;
			
			i3::generic_clear(output);

			InIt beg_it = rng.begin();
			InIt end_it = rng.end();
			InIt it		= beg_it;

			InIt front_end;
			InIt env_begin;
			
			i3::stack_basic_string<BuffSizeType, Ch>			input_env;
			i3::stack_basic_string<BuffSizeType, Ch>			output_env;
			

			while( ( it = i3::find(it, end_it, Ch('$')) ) != end_it )
			{
				front_end = it;

				if (++it == end_it) 
					break;

				if ( *it != Ch('(') )		// 루프종료하지말고, 다시 해야한다..
					continue;
								
				env_begin = ++it;
				
				it = i3::find( env_begin, end_it, Ch(')') );

				if ( it == end_it )			// 이경우엔 뒤에 $, (가 잘들어오더라도 모두 무시한다..
					break;
				
				i3::generic_append_range(output, beg_it, front_end);

				input_env.assign( env_begin, it );
				i3::get_environment_variable(input_env, output_env);

				i3::generic_append_range(output, output_env);

				beg_it = ++it;		// ')' 다음..
			}
			
			//
			i3::generic_append_range(output, beg_it, end_it);			// beg_it값은 루프중에 바뀐값이어야한다..
			//
		}
	}

	template<class Rng, class OutSeq>  inline
	typename disable_if_iterator<OutSeq>::type 
	resolve_path_from_environment_variable(const Rng& input, OutSeq& output)
	{
		detail::resolve_path_from_environment_variable_impl(i3::as_literal(input), output);
	}
	
}