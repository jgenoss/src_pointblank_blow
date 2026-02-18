#pragma once

#include "../../algorithm/rfind.h"

namespace i3
{

	// SPRange = SinglePassRange
	namespace range
	{
		template<class BidRange, class Value> inline 
		typename range_iterator<BidRange>::type rfind(BidRange& r, const Value& v)
		{
			return i3::rfind(i3::begin(r), i3::end(r), v);
		}

		template<class BidRange, class Value> inline 
		typename range_iterator<const BidRange>::type rfind(const BidRange& r, const Value& v)
		{
			return i3::rfind(i3::begin(r), i3::end(r), v);
		}
	}

}

namespace i3range = i3::range;