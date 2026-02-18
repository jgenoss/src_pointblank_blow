#pragma once

#include "find_format_all.h"
#include "first_finder.h"
#include "const_formatter.h"
#include "../compare/is_iequal.h"

//
//  이터레이터 버전의 경우 끝에 널값 추가를 하지 않는 상태이다..( 이부분은 문자열포인터가 아닌 경우까지 감안되서임..)
//  우선, 외부에서 수동적으로 널값을 셋팅할것.

namespace i3
{
	template<class OutIt, class InputRng, class SearchRng, class RepRng> inline
	OutIt replace_all_copy(OutIt out, const InputRng& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_all_copy(out, in, first_finder(s), const_formatter(rep) );
	}
	
	template<class Seq, class SearchRng, class RepRng> inline
	Seq replace_all_copy(const Seq& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_all_copy(in, first_finder(s), const_formatter(rep) );
	}

	template<class Seq, class SearchRng, class RepRng> inline
	void replace_all(Seq& inout, const SearchRng& s, const RepRng& rep)
	{
		find_format_all(inout, first_finder(s), const_formatter(rep) );
	}

////////////////// 아래는 대소문자구별없는 버전..

	template<class OutIt, class InputRng, class SearchRng, class RepRng> inline
	OutIt ireplace_all_copy(OutIt out, const InputRng& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_all_copy(out, in, first_finder(s, is_iequal() ), const_formatter(rep) );
	}
	
	template<class Seq, class SearchRng, class RepRng> inline
	Seq ireplace_all_copy(const Seq& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_all_copy(in, first_finder(s, is_iequal() ), const_formatter(rep) );
	}

	template<class Seq, class SearchRng, class RepRng> inline
	void ireplace_all(Seq& inout, const SearchRng& s, const RepRng& rep)
	{
		find_format_all(inout, first_finder(s, is_iequal() ), const_formatter(rep) );
	}


}
