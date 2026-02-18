#pragma once

// C++11부터 표준

namespace i3
{

	template<typename InIt, typename OutIt, typename Pred1> inline
	OutIt copy_while ( InIt first, InIt last, OutIt result, Pred1 p )
	{
		for ( ; first != last && p(*first); ++first )
			*result++ = *first;
		return result;
	}
	
	// 아래 코드는 실제 복사를 수행하지 않을 것이다.
	// find_not_if 
	//
	template<typename InOutIt, typename Pred1> inline
	InOutIt copy_while ( InOutIt first, InOutIt last, Pred1 p )
	{
		for ( ; first != last && p(*first); ++first ) {}
		return first;
	}
	
}