#pragma once

#include "../../algorithm/find.h"

namespace i3
{

	// SPRange = SinglePassRange
	namespace range
	{
		template<class FwdRange, class Value> inline 
		typename range_iterator<FwdRange>::type find_backward(FwdRange& r, const Value& v)
		{
			typename i3::range_reverse_iterator<FwdRange>::type rit = i3::find( rbegin(r), rend(r), v);
			if ( rit == rend(r) ) 
				return end(r);
			return (++rit).base();
		}

		template<class FwdRange, class Value> inline 
		typename range_iterator<const FwdRange>::type find_backward(const FwdRange& r, const Value& v)
		{
			typename i3::range_reverse_iterator<const FwdRange>::type rit = i3::find( rbegin(r), rend(r), v);
			if ( rit == rend(r) )
				return end(r);
			return (++rit).base();
		}
	}

}

namespace range = i3::range;