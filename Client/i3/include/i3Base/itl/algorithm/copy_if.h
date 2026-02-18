#pragma once

// C++11부터 표준

namespace i3
{
	template<typename InIt, typename OutIt, typename Pred1> inline
	OutIt copy_if( InIt first, InIt last, OutIt result, Pred1 p )
	{
		for ( ; first != last; ++first )
			if (p(*first))
				*result++ = *first;
		return result;
	}
	
	// 내부 복사용..

	template<typename InOutIt, typename Pred1> inline
	InOutIt copy_if( InOutIt first, InOutIt last, Pred1 p )
	{
		// 다른게 나올때까지는 복사동작이 필요없음..
		for ( ; first != last ; ++first)
			if (!p(*first))
				break;

		if ( first == last ) return first;
		InOutIt f1 = first;
		return copy_if( ++f1, last, first, p);
	}

}