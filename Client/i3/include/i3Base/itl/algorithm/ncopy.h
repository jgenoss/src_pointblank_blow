#pragma once

//
// 원래 std::algorithm에는 없음..
// dest인수의 복사량 제한을 위해서 만든다...
//
#include "../iterator/iterator_traits.h"
#include "copy.h"

namespace i3
{
	template <class InIt, class OutIt> inline
	OutIt ncopy(InIt f, InIt l, OutIt dest, std::size_t dest_max_size)			// 버퍼사이즈가 아니라...복사갯수제한이다..
	{
		typedef typename i3::iterator_difference<InIt>::type		diff_t;
		diff_t dist = i3::distance(f, l);
		if ( dist > dest_max_size )
		{
			l = f;		i3::advance(l, dest_max_size);
		}
		return i3::copy(f, l, dest);
	}
}
