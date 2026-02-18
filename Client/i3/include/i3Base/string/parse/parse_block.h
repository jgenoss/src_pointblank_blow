#pragma once

//
// 이스케이프 시퀀스 앞 블록  뒷블록으로 나눠서.....
// 필요한 위치들을 알려주고, 다음 위치로 넘기는 함수..
//
#include "../../itl/iterator/iterator_traits.h"

namespace i3
{
	template<class It>
	bool	parse_block(	It beg, It end, 
							typename i3::iterator_value<It>::type open_char,
							typename i3::iterator_value<It>::type close_char,
							It& block_beg, It& block_end)
	{
		typedef typename i3::iterator_value<It>::type	Ch;
		
		beg = i3::find(beg, end, open_char);
		block_beg = beg;
		if ( beg == end )		// '{' 없으면 바로 실패..
		{
			block_end = end;	return false;
		}
		
	//	beg = i3::find(++beg, end, close_char);	// '{' 다음 문자열부터.. (버그가 있으므로 주석하고 아래와 같이 변경)
	//////////////////////////////////////////////
		for ( ++beg ; beg != end ; ++beg)
		{
			if (*beg == close_char)
				break;
			else
			if (*beg == open_char)
				block_beg = beg;		// block_beg 변경..( { { 와 같이 쌍이 맞지않는 시작 중괄호 중복시 시작블록 위치 변경)
		}
	//////////////////////////////////////////////
		block_end = beg;		
		if (beg == end)
		{
			block_beg = end;
			return false;
		}

		++block_end;
		return true;					//  '}' 없으면 그대로 실패.
	}





}