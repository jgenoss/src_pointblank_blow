#pragma once

#include "../../algorithm/find.h"

namespace i3
{
	
	// SPRange = SinglePassRange
	namespace range
	{
		template<class SPRange, class Value> inline 
		typename range_iterator<SPRange>::type find(SPRange& r, const Value& v)
		{
			return i3::find(i3::begin(r), i3::end(r), v);
		}

		template<class SPRange, class Value> inline 
		typename range_iterator<const SPRange>::type find(const SPRange& r, const Value& v)
		{
			return i3::find(i3::begin(r), i3::end(r), v);
		}
	}

}

namespace i3range = i3::range;