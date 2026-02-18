#pragma once


#include "../../algorithm/rfind_not.h"

namespace i3
{

	// SPRange = SinglePassRange
	namespace range
	{
		template<class BidRange, class Value> inline 
		typename range_iterator<BidRange>::type rfind_not(BidRange& r, const Value& v)
		{
			return i3::rfind_not(i3::begin(r), i3::end(r), v);
		}

		template<class BidRange, class Value> inline 
		typename range_iterator<const BidRange>::type rfind_not(const BidRange& r, const Value& v)
		{
			return i3::rfind_not(i3::begin(r), i3::end(r), v);
		}
	}

}

namespace i3range = i3::range;