#pragma once

// C++11부터 표준

namespace i3
{

	template<typename InIt, typename OutIt, typename Pred1> inline
	OutIt copy_until( InIt first, InIt last, OutIt result, Pred1 p )
	{
		for ( ; first != last && !p(*first); ++first )
			*result++ = *first;
		return result;
	}
	
	//
	// 아래 코드는 실제 복사를 수행하지 않는다.
	// find_if와 같다..
	//
	template<typename InOutIt, typename Pred1> inline
	InOutIt copy_until( InOutIt first, InOutIt last, Pred1 p )
	{
		for ( ; first != last && !p(*first); ++first ) {}
		return first;
	}

}