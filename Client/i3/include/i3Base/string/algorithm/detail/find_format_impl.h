#pragma once
#include "../../itl/range/generic_insert_iterators.h"

namespace i3
{
	
	namespace detail
	{
		template<class OutIt, class Input, class FindResult, class FormatResult> inline
		OutIt find_format_copy_impl2( OutIt out, const Input& input, const FindResult& find_res, const FormatResult& format_res)
		{
		
			// 진짜 구현은 다음 세줄 뿐임...많은 여타 코드들은 구현코드가 아님..
			out = i3::copy( begin(input), begin(find_res), out );
			out = i3::copy( begin(format_res), end(format_res), out);
			out = i3::copy( end(find_res), end(input), out);
			//
			return out;
		}

		template<class OutIt, class Input, class Formatter, class FindResult> inline
		OutIt find_format_copy_impl(OutIt out, const Input& input, Formatter formatter, const FindResult& find_res)
		{
			if ( i3::empty(find_res) )
				return i3::copy( begin(input), end(input), out);
			
			return find_format_copy_impl2(out, input, find_res, formatter(find_res));
		}


		template<class Input, class Formatter, class FindResult, class FormatResult> inline
		Input find_format_copy_impl2( const Input& input, const FindResult& find_res, const FormatResult& format_res)
		{
			
			// 진짜 구현은 다음 세줄 뿐임..
			Input out;		// Input이 insert멤버함수가 존재하는 sequence여야 하는 제약조건이 있음..
			
			i3::generic_insert_iterators(out, end(out), begin(input), begin(find_res) );
			i3::generic_insert_iterators(out, end(out), begin(format_res), end(format_res));
			i3::generic_insert_iterators(out, end(out), end(find_res), end(input));
			
			return out;
		}

		template<class Input, class Formatter, class FindResult> inline
		Input find_format_copy_impl( const Input& input, Formatter formatter, const FindResult& find_res )
		{
			if (i3::empty(find_res) )
				return input;				// 그냥 리턴하면 복사생성..

			return find_format_copy_impl2(input, find_res, formatter(find_res) );
		}
		

		template<class Input, class FindResult, class FormatResult> inline
		void find_format_impl2(Input& inout, const FindResult& find_res, const FormatResult& format_res)
		{
			i3::generic_replace_range(inout, begin(find_res), end(find_res), begin(format_res), end(format_res));			
		}


		template<class Input, class Formatter, class FindResult> inline
		void find_format_impl(Input& inout, Formatter formatter, const FindResult& find_res)
		{
			if (i3::empty(find_res)) 
				return;
			
			find_format_impl2(inout, find_res, formatter(find_res));
		}


	}
}