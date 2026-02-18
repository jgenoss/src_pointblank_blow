#pragma once

//
// strtol을 잘 래핑하면 된다... 끝..
//
//

#include "strtol.h"

namespace i3
{
	template<class Range> inline 
	int atoi(const Range& rng)
	{
		return int( i3::strtol(rng, 10) );			// 10으로 고정한다..( crt atoi와의 호환성문제)
	}	
}
