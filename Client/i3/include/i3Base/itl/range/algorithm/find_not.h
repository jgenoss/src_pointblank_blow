#pragma once


#include "../../algorithm/find_not.h"

namespace i3
{

	// SPRange = SinglePassRange
	namespace range
	{
		template<class SPRange, class Value> inline 
		typename range_iterator<SPRange>::type find_not(SPRange& r, const Value& v)
		{
			return i3::find_not(i3::begin(r), i3::end(r), v);
		}

		template<class SPRange, class Value> inline 
		typename range_iterator<const SPRange>::type find_not(const SPRange& r, const Value& v)
		{
			return i3::find_not(i3::begin(r), i3::end(r), v);
		}
	}

}

namespace i3range = i3::range;