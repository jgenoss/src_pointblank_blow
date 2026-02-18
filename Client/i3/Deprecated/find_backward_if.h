#pragma once

#include "../../algorithm/find_if.h"

namespace i3
{

	// SPRange = SinglePassRange
	namespace range
	{
		template<class FwdRange, class Pred1> inline 
		typename range_iterator<FwdRange>::type find_backward_if(FwdRange& r, Pred1 pr)
		{
			typename i3::range_reverse_iterator<FwdRange>::type rit = i3::find_if( rbegin(r), rend(r), pr);
			if ( rit == rend(r) ) 
				return end(r);
			return (++rit).base();
		}

		template<class FwdRange, class Pred1> inline 
		typename range_iterator<const FwdRange>::type find_backward_if(const FwdRange& r, Pred1 pr)
		{
			typename i3::range_reverse_iterator<const FwdRange>::type rit = i3::find_if( rbegin(r), rend(r), pr);
			if ( rit == rend(r) )
				return end(r);
			return (++rit).base();
		}
	}

}

namespace range = i3::range;
