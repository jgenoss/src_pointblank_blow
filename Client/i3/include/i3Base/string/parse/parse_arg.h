#pragma once

//
// parse_cmd의 인풋문자열에서 커맨드문자열이 일치하면 그다음의 인수 처리를 하기 위한 함수객체를 만든다..
//
// i3::tie와 파싱 부분을 연결해서 처리해본다..		tie에 해당되는 문자열순서에 tie의 타입대로 파싱한다..
//
#include "../ext/atof.h"
#include "../../itl/algorithm/find_first_of.h"
#include "../ext/generic_find_first_not_of.h"
#include "../../itl/tuple/tuple.h"
#include "../ext/make_generic_literal.h"
#include "../ext/atoi.h"

namespace i3
{
	// tie 전용 파서를 만들고, 이걸 활용하는 쪽으로 해본다..
	//			const Ch* delimit = make_generic_literal(Ch, " \n\t,:");			// i3기존 루틴과의 호환성때문에 일단 이렇게..

	namespace detail
	{
		
		template<class It, class SepIterRange>
		It	find_arg_parser_token(It beg, It end, It& out_beg, It& out_end, const SepIterRange& sep)
		{
			// 앞쪽 화이트스페이스 제거하고, 뒷쪽은 콤마 토크닝으로 끊는다...
			out_beg = beg;

			typedef typename i3::iterator_value<It>::type		Ch;

			out_end = i3::find_first_of(beg, end, sep.begin(), sep.end() );	// 찾은 구분자...이것 다음부터 처리하는게 좋다..
			
			It next_beg = i3::find_non_whitespace(out_end, end);
			
			if (next_beg != end) 
			{	
				if ( i3::find(sep.begin(), sep.end(), *next_beg) != sep.end() )
					++next_beg;
			}

			return next_beg;
		}
		
		template<class It>
		struct tie_arg_parser_custom
		{
			void parse_value(It beg, It end, float& val)
			{
				typedef typename i3::iterator_value<It>::type Ch;
				i3::stack_basic_string<mpl::size_t<32>, Ch>	tmp(beg, end);		// 널값이 필요하기 때문에 이렇게 처리..
				val = i3::atof(tmp);
			}
			void parse_value(It beg, It end, int& val)
			{
				val = i3::atoi( i3::make_iterator_range(beg, end) );
			}
			void parse_value(It beg, It end, BYTE& val)
			{
				int res = i3::atoi( i3::make_iterator_range(beg, end) );
				val = ( res > 255 ) ? 255 : BYTE(res);
			}
			
			void parse_value(It beg, It end, ...) { }							// 구현을 비운다..
		};

		template<class It, class SepIterRange>
		struct tie_arg_parser_entity : tie_arg_parser_custom<It>
		{

			typedef tie_arg_parser_custom<It>		base_type;

			tie_arg_parser_entity(It beg, It end, const SepIterRange& rng) : m_beg(beg), m_end(end), m_sep_rng(rng), m_parse_count(0) {}
			
			template<class T>
			void	operator()(T& result)
			{
				if (m_beg == m_end) return;
		
			//	m_beg = i3::find_non_whitespace(m_beg, m_end);
				m_beg = i3::generic_find_first_not_of(m_beg, m_end, m_sep_rng );	
						
				It value_begin, value_end;
				m_beg = find_arg_parser_token(m_beg, m_end, value_begin, value_end, m_sep_rng);
				
				base_type::parse_value(value_begin, value_end, result);
				++m_parse_count;
			}

			It m_beg;
			It m_end;
			size_t m_parse_count;
			const SepIterRange& m_sep_rng;
		};
		

		template<class InIt, class Tie, class Sep> inline
		size_t parse_arg_impl(InIt beg, InIt end, const Tie& t, const Sep& rng)
		{
			tie_arg_parser_entity<InIt, Sep>	tuple_fn(beg, end, rng);
			i3::for_each_tuple(t, tuple_fn);		// 주체가 반복자가 되지 않고, 튜플타입순서로 되는것이 기존 구현과의 차이가 된다..
			return tuple_fn.m_parse_count;
		}
		
	}
	

	template<class Ch> inline
	const i3::iterator_range<const Ch*>	default_separator() 
	{
		const Ch* lit = i3::make_generic_literal(Ch, " \n\t,:");
		return i3::iterator_range<const Ch*>( lit, lit+5 );
	}

	template<class InIt, class Tie, class Sep> inline
	size_t parse_arg(InIt arg_start, InIt arg_end, const Tie& tie, const Sep& rng) 
	{
		return detail::parse_arg_impl(arg_start, arg_end, tie, i3::as_literal(rng) );
	}
	
	template<class InIt, class Tie> inline
	size_t parse_arg(InIt arg_start, InIt arg_end, const Tie& tie)
	{
		typedef typename i3::iterator_value<InIt>::type		Ch;
		return detail::parse_arg_impl(arg_start, arg_end, tie, default_separator<Ch>() );
	}

}