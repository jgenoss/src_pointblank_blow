#pragma once

#include "../../algorithm/find_if.h"
#include "../iterator.h"

namespace i3
{
	// SPRange = SinglePassRange
	namespace range
	{
		template<class SPRange, class Pred1> inline 
		typename range_iterator<SPRange>::type find_if(SPRange& r, Pred1 pr)
		{
			return i3::find_if(i3::begin(r), i3::end(r), pr);
		}
		
		template<class SPRange, class Pred1> inline 
		typename range_iterator<const SPRange>::type find_if(const SPRange& r, Pred1 pr)
		{
			return i3::find_if(i3::begin(r), i3::end(r), pr);
		}
	}
	
}

namespace i3range = i3::range;