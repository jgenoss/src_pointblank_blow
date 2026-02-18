#pragma once

// 응용루틴일것이므로 i3 namespace대신에 전역공간에서 구현해본다..



#include "string/parse/parse_block.h"
#include "string/parse/parse_cmd.h"
#include "string/ext/make_generic_literal.h"

#include "itl/range/generic_append_range.h"
#include "itl/range/generic_clear.h"
#include "itl/range/generic_reserve.h"

namespace detail 
{
	
	template<class It, class SeqOrLit>	// Seq의 경우 참조형이 붙어있어야한다.
	bool RemoveColorStringRecur(It& pos, It end, int depth, SeqOrLit refined)
	{
		typedef typename i3::iterator_value<It>::type	Ch;

		bool res = false;

		It block_beg = end;		It block_end = end;
		bool bBlock = i3::parse_block(pos, end, Ch('{'), Ch('}'), block_beg, block_end);

		i3::generic_append_range(refined, pos, block_beg);	// 블록 시작전까지는 refined에 범위복사..
		pos = block_end;

		while (bBlock)
		{
			It cmd_beg = block_beg;		++cmd_beg;	
			It cmd_end = block_end;		--cmd_end;
			It arg_start = cmd_end;		It arg_end   = cmd_end;
			
			i3::iterator_range<const Ch*> cmd_rng = i3::make_iterator_range(cmd_beg, cmd_end);

			if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "col"), arg_start, arg_end) )
			{
				RemoveColorStringRecur<It, SeqOrLit>(pos, end, ++depth, refined);
				res = true;
			}
			else
			if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "/col"), arg_start, arg_end) )
			{
				if (depth != 0)
					return true;
				res = true;
			}
			else	// 커맨드가 이상한 경우..일반 문자열로 간주하고 다음 진행....
			{
				i3::generic_append_range(refined, block_beg, block_end);
				// 블록 시작전까지는 refined에 범위복사..
			}

			// 블록 뒷부분 처리
			block_beg = end;	block_end = end;
			bBlock = i3::parse_block(pos, end, Ch('{'), Ch('}'), block_beg, block_end);
			i3::generic_append_range(refined, pos, block_beg);	// 블록 시작전까지는 refined에 범위복사..
			pos = block_end;
		}

		return res;
	}

	template<class ItRng, class SeqOrLit>	// Seq의 경우 참조형이 붙어있어야한다.
	bool RemoveColorStringImpl(const ItRng& rng, SeqOrLit refined)
	{
		i3::generic_clear(refined);

		const size_t rng_len = rng.size();
		i3::generic_reserve(refined, rng_len);		

		typedef typename i3::range_iterator<const ItRng>::type	range_iterator;
		range_iterator pos = rng.begin();
		return RemoveColorStringRecur<range_iterator, SeqOrLit>( pos, rng.end(), 0, refined );
	}
	
	// 필요가 없다면, 아무것도 하지 않는 버전도 만들어야 한다..
	template<class ItRng, class SeqOrLit>	// Seq의 경우 참조형이 붙어있어야한다.
	bool RemoveColorStringImplIfNeeded(const ItRng& rng, SeqOrLit refined)
	{
		i3::generic_clear(refined);	
	
		// 이경우 첫 파싱의 별도 구현이 필요함..
		typedef typename i3::range_iterator<const ItRng>::type It;
		typedef typename i3::iterator_value<It>::type	Ch;

		It pos = rng.begin();
		It end   = rng.end();

		It block_beg = end;		It block_end = end;
		
		if ( i3::parse_block(pos, end, Ch('{'), Ch('}'), block_beg, block_end) == false)
			return false;

		It cmd_beg = block_beg;		++cmd_beg;	
		It cmd_end = block_end;		--cmd_end;

		It arg_start = cmd_end;		It arg_end   = cmd_end;
				
		i3::iterator_range<It> cmd_rng = i3::make_iterator_range(cmd_beg, cmd_end);

		if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "col"), arg_start, arg_end) )
		{
			const size_t rng_len = rng.size();
			i3::generic_reserve(refined, rng_len);		
			i3::generic_append_range(refined, pos, block_beg);

			pos = block_end;
			// 블록 내부 재귀..
			RemoveColorStringRecur<It, SeqOrLit>(pos, end, 1, refined );
			// 블록 뒷부분 재귀
			RemoveColorStringRecur<It, SeqOrLit>(pos, end, 0, refined );
			return true;
		}
		else if (  i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "/col"), arg_start, arg_end) )
		{
			const size_t rng_len = rng.size();
			i3::generic_reserve(refined, rng_len);			
			i3::generic_append_range(refined, pos, block_beg);
			
			pos = block_end;
			// 블록 바깥만 처리하면 됨..
			RemoveColorStringRecur<It, SeqOrLit>(pos, end, 0, refined );
			//
			return true;		// '/col'은 색상 해석되진 않지만, 문자열에서 제외되어야한다..
			//	
		}

		return false;
	}


	template<class SeqOrLit> __forceinline
	bool RemoveColorStringImpl(SeqOrLit inout)
	{
		typedef typename i3::range_value<typename i3::remove_reference<SeqOrLit>::type >::type	Ch;

		typedef i3::stack_basic_string<mpl::size_t<128>, Ch>	stack_string_type;
		stack_string_type  temp;
		bool res = RemoveColorStringIfNeeded( inout, temp);

		if (res)
			i3::generic_string_copy(inout, temp);

		return res;
	}

}

// 이 함수는 강제 실행된다...
template<class Range, class Lit> __forceinline typename i3::enable_if_iterator<Lit,
bool>::type	RemoveColorString(	const Range& input, Lit refined)
{
	typedef typename i3::get_iterator_range_type<const Range>::type ItRng;

	return detail::RemoveColorStringImpl< ItRng, Lit>(i3::as_literal(input), refined );
}

template<class Range, class Seq> __forceinline typename i3::disable_if_iterator<Seq,
bool>::type	RemoveColorString(	const Range& input, Seq& refined)
{
	typedef typename i3::get_iterator_range_type<const Range>::type ItRng;

	return detail::RemoveColorStringImpl<ItRng, Seq&>(i3::as_literal(input), refined );	// 참조 명기..
}

template<class Range, class Ch>  inline 
bool	RemoveColorString(	const Range& input, i3::rc_basic_string<Ch>& refined)
{
	typedef i3::stack_basic_string<mpl::size_t<128>, Ch>	stack_string_type;
	stack_string_type  temp;

	bool res = RemoveColorStringIfNeeded( i3::as_literal(input), temp);

	if (res)
		refined = temp;
	else
		refined = input;

	return res;
}



// 이 함수는, 이스케이프 커맨드가 없는 것이 확실하다면 출력 컨테이너에 아무것도 넣지 않고 빠져나오도록 한다..
template<class Range, class Lit> __forceinline typename i3::enable_if_iterator<Lit,
bool>::type	RemoveColorStringIfNeeded( const Range& input, Lit refined)
{
	typedef typename i3::get_iterator_range_type<const Range>::type ItRng;
	return detail::RemoveColorStringImplIfNeeded<ItRng, Lit>(i3::as_literal(input), refined);
}

template<class Range, class Seq> __forceinline typename i3::disable_if_iterator<Seq,
bool>::type	RemoveColorStringIfNeeded( const Range& input, Seq& refined)
{
	typedef typename i3::get_iterator_range_type<const Range>::type ItRng;
	return detail::RemoveColorStringImplIfNeeded<ItRng, Seq&>(i3::as_literal(input), refined);
}

template<class Range, class Ch>  inline 
bool	RemoveColorStringIfNeeded(	const Range& input, i3::rc_basic_string<Ch>& refined)
{
	typedef i3::stack_basic_string<mpl::size_t<128>, Ch>	stack_string_type;
	stack_string_type  temp;

	bool res = RemoveColorStringIfNeeded( i3::as_literal(input), temp);

	if (res)
		refined = temp;

	return res;
}


template<class Lit> __forceinline typename i3::enable_if_iterator<Lit,
bool>::type RemoveColorString(Lit inout)
{
	return detail::RemoveColorStringImpl<Lit>(inout);
}

template<class Seq> __forceinline typename i3::disable_if_iterator<Seq,
bool>::type RemoveColorString(Seq& inout)
{
	return detail::RemoveColorStringImpl<Seq&>(inout);
}
