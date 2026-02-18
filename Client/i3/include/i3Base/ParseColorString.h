#pragma once

// 응용루틴일것이므로 i3 namespace대신에 전역공간에서 구현해본다..

// 목표 구현..
//
// 1. 전체문자열을 받아서 출력될 실문자열을 뽑아낸다.

//
// 2. 문자열 크기 색상 배열 대신, 색상 테이블을 두고, 색상의 테이블 인덱스 배열로 처리하는 방식으로 변경했음..(2014.12.17.수빈)
//  --> 이렇게 변경하면 억세스 속도는 살짝 떨어질수 있지만, 색상 콘트롤은 좀더 쉬워진다..
// 

// 3. 실문자열 인덱스를 넣으면 원본문자열의 인덱스를 알려줄수 있는 mapper배열을 넣는다.
// 4. 시작블록과 끝블록 처리를 하되...스택 구현이 될수 있도록 처리한다..
//
// 
//
// i3UIText구현내용과 달리 기본 함수는 이스케이프 블록이 없더라도 강제 실행되는데...
// 속도저하는 없지만...불필요한 메모리사용이 될수 있다..
//
// 꼭 필요할때만 메모리를 사용하도록 조정한 함수를 따로 만듬...
//
//

#include "string/parse/parse_block.h"
#include "string/parse/parse_cmd.h"
#include "string/parse/parse_arg.h"
#include "string/ext/make_generic_literal.h"

namespace detail 
{
	
	template<class It, class Ch> 
	bool ParseColorStringRecur(It orig, It end, It& pos, DWORD curr_clr_idx, i3::basic_string<Ch>& refined, 
		i3::vector<DWORD>& clr_table, i3::vector<DWORD>& clr_idx_array, i3::vector<size_t>& to_input_map)
	{

		bool res = false;

		It start = pos;

		It block_beg = end;		It block_end = end;
		
		bool bBlock = i3::parse_block(start, end, Ch('{'), Ch('}'), block_beg, block_end);
			
		pos = block_end;

		refined.insert(refined.end(), start, block_beg);			// 블록 시작전까지는 refined에 범위복사..

		const size_t num_block_outside = i3::distance(start, block_beg);

		clr_idx_array.insert(clr_idx_array.end(), num_block_outside, curr_clr_idx);

		const size_t dist_orig_start = i3::distance(orig, start);
		for (size_t i = 0 ; i < num_block_outside ; ++i )
		{
			to_input_map.push_back(dist_orig_start  + i);			
		}
		
		if (bBlock)
		{
			It cmd_beg = block_beg;		++cmd_beg;	
			It cmd_end = block_end;		--cmd_end;
			It arg_start = cmd_end;		It arg_end   = cmd_end;
			
			i3::iterator_range<const Ch*> cmd_rng = i3::make_iterator_range(cmd_beg, cmd_end);

			if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "col"), arg_start, arg_end) )
			{
				DWORD next_clr = clr_table[curr_clr_idx];							// default_clr 
				BYTE* rgba = reinterpret_cast<BYTE*>(&next_clr);		
				i3::parse_arg(arg_start, arg_end, i3::tie(rgba[0],rgba[1],rgba[2],rgba[3]) );
				
				clr_table.push_back(next_clr);

				ParseColorStringRecur(orig, end, pos, DWORD(clr_table.size() ) -1 , refined, clr_table, clr_idx_array, to_input_map );
					
				res = true;
			}
			else
			if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "/col"), arg_start, arg_end) )
			{
				if (orig != start)
					return true;

				res = true;
			}
			else	// 커맨드가 이상한 경우..일반 문자열로 간주하고 다음 진행....
			{
				refined.insert(refined.end(), block_beg, block_end);			// 블록 시작전까지는 refined에 범위복사..

				const size_t num_block = i3::distance(block_beg, block_end);

				clr_idx_array.insert(clr_idx_array.end(), num_block, curr_clr_idx);

				const size_t dist_orig_block_beg = i3::distance(orig, block_beg);

				for (size_t i = 0 ; i < num_block ; ++i )
				{
					to_input_map.push_back(dist_orig_block_beg + i);			
				}
			}

			// 블록 뒷부분 처리를 재귀로 함..
			res |= ParseColorStringRecur(orig, end, pos, curr_clr_idx, refined, clr_table, clr_idx_array, to_input_map);
			//	
		}

		return res;
	}

	template<class ItRng, class Ch> 
	bool ParseColorStringImpl(const ItRng& rng, DWORD default_clr, i3::basic_string<Ch>& refined, 
		i3::vector<DWORD>& clr_table, i3::vector<DWORD>& clr_idx_array, i3::vector<size_t>& to_input_map)
	{
		refined.clear();		clr_table.clear();		clr_idx_array.clear();		to_input_map.clear();

		const size_t rng_len = rng.size();
		refined.reserve(rng_len);		clr_idx_array.reserve(rng_len);	to_input_map.reserve(rng_len);

		clr_table.push_back(default_clr);		// 일단 디폴트색상을 테이블 0번에 넣는다.
		
		typedef typename i3::range_iterator<const ItRng>::type	range_iterator;
		range_iterator pos = rng.begin();
		return ParseColorStringRecur( rng.begin(), rng.end(), pos, 0, refined, clr_table, clr_idx_array, to_input_map);
	}
	
	// 필요가 없다면, 아무것도 하지 않는 버전도 만들어야 한다..
	template<class ItRng, class Ch>
	bool ParseColorStringImplIfNeeded(const ItRng& rng, DWORD default_clr, i3::basic_string<Ch>& refined, 
		i3::vector<DWORD>& clr_table, i3::vector<DWORD>& clr_idx_array, i3::vector<size_t>& to_input_map)
	{
		refined.clear();		clr_table.clear();		clr_idx_array.clear();		to_input_map.clear();
	
		// 이경우 첫 파싱의 별도 구현이 필요함..
		typedef typename i3::range_iterator<const ItRng>::type It;

		It start = rng.begin();
		It end   = rng.end();

		It block_beg = end;		It block_end = end;
		bool bBlock = i3::parse_block(start, end, Ch('{'), Ch('}'), block_beg, block_end);
		
		if (!bBlock)
			return false;
		
		It pos = block_end;

		It cmd_beg = block_beg;		++cmd_beg;	
		It cmd_end = block_end;		--cmd_end;
		It arg_start = cmd_end;		It arg_end   = cmd_end;
		
		i3::iterator_range<const Ch*> cmd_rng = i3::make_iterator_range(cmd_beg, cmd_end);

		if ( i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "col"), arg_start, arg_end) )
		{
			const size_t rng_len = rng.size();
			refined.reserve(rng_len);		clr_idx_array.reserve(rng_len);	to_input_map.reserve(rng_len);
			clr_table.push_back(default_clr);

			refined.insert(refined.end(), start, block_beg);			// 블록 시작전까지는 refined에 범위복사..

			const size_t num_block_outside = i3::distance(start, block_beg);

			clr_idx_array.insert(clr_idx_array.end(), num_block_outside, 0);
			
			for (size_t i = 0 ; i < num_block_outside ; ++i )
			{
				to_input_map.push_back(i);			
			}
						
			DWORD next_clr = default_clr;							// default_clr 
			BYTE* rgba = reinterpret_cast<BYTE*>(&next_clr);		
			i3::parse_arg(arg_start, arg_end, i3::tie(rgba[0],rgba[1],rgba[2],rgba[3]) );
			
			clr_table.push_back(next_clr);

			// 블록 내부 재귀..
			ParseColorStringRecur(start, end, pos, 1, refined, clr_table, clr_idx_array, to_input_map );
					
			// 블록밖으로 나올때 다시 재귀..
			ParseColorStringRecur(start, end, pos, 0, refined, clr_table, clr_idx_array, to_input_map);

			return true;
		}
		else if (  i3::parse_cmd(cmd_rng, i3::make_generic_literal(Ch, "/col"), arg_start, arg_end) )
		{
			const size_t rng_len = rng.size();
			refined.reserve(rng_len);		clr_idx_array.reserve(rng_len);	to_input_map.reserve(rng_len);
			clr_table.push_back(default_clr);

			refined.insert(refined.end(), start, block_beg);			// 블록 시작전까지는 refined에 범위복사..

			const size_t num_block_outside = i3::distance(start, block_beg);

			clr_idx_array.insert(clr_idx_array.end(), num_block_outside, 0);	
			
			for (size_t i = 0 ; i < num_block_outside ; ++i )
			{
				to_input_map.push_back(i);			
			}

			// 블록 바깥만 처리하면 됨..
			ParseColorStringRecur(start, end, pos, 0, refined, clr_table, clr_idx_array, to_input_map);
			//

			return true;				// '/col'은 색상 해석되진 않지만, 문자열에서 제외되어야한다..
			//	
		}
		
		// 커맨드가 이상한 경우..일반 문자열로 간주하고 다음 진행....
		
		const size_t rng_len = rng.size();
		refined.reserve(rng_len);	clr_idx_array.reserve(rng_len); to_input_map.reserve(rng_len);
		clr_table.push_back(default_clr);

		refined.insert(refined.end(), start, block_end);			// 처음부터 블록 끝까지 refined에 범위복사..

		const size_t num_start_blockend = i3::distance(start, block_end);

		clr_idx_array.insert(clr_idx_array.end(), num_start_blockend, 0);

		for (size_t i = 0 ; i < num_start_blockend ; ++i )
		{
			to_input_map.push_back(i);			
		}
			
		ParseColorStringRecur(start, end, pos, 0, refined, clr_table, clr_idx_array, to_input_map);
		
		return true;
	}
}

// 이 함수는 강제 실행된다...
template<class Range, class Ch> __forceinline
bool	ParseColorString(	const Range& input, DWORD default_clr, i3::basic_string<Ch>& refined, 
						 i3::vector<DWORD>& clr_table, i3::vector<DWORD>& clr_idx_array, i3::vector<size_t>& to_input_map)
{
	return detail::ParseColorStringImpl(i3::as_literal(input), default_clr, refined, clr_table, clr_idx_array, to_input_map);
}


// 이 함수는, 이스케이프 커맨드가 없는 것이 확실하다면 출력 컨테이너에 아무것도 넣지 않고 빠져나오도록 한다..
template<class Range, class Ch> __forceinline
bool	ParseColorStringIfNeeded( const Range& input, DWORD default_clr, i3::basic_string<Ch>& refined, 
							i3::vector<DWORD>& clr_table, i3::vector<DWORD>& clr_idx_array, i3::vector<size_t>& to_input_map)
{
	return detail::ParseColorStringImplIfNeeded(i3::as_literal(input), default_clr, refined, clr_table, clr_idx_array, to_input_map);
}


