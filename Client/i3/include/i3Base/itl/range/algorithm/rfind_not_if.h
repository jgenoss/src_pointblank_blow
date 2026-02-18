#pragma once

#include "../../algorithm/rfind_not_if.h"
#include "../iterator.h"

namespace i3
{
	// SPRange = SinglePassRange
	namespace range
	{
		template<class BidRange, class Pred1> inline 
		typename range_iterator<BidRange>::type rfind_not_if(BidRange& r, Pred1 pr)
		{
			return i3::rfind_not_if(i3::begin(r), i3::end(r), pr);
		}

		template<class BidRange, class Pred1> inline 
		typename range_iterator<const BidRange>::type rfind_not_if(const BidRange& r, Pred1 pr)
		{
			return i3::rfind_not_if(i3::begin(r), i3::end(r), pr);
		}
	}

}

namespace i3range = i3::range;