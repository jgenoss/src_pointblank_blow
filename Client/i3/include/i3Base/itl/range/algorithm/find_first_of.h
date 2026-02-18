#pragma once

#include "../../algorithm/find_first_of.h"

namespace i3
{
	
	// SPRange = SinglePassRange
	namespace range
	{
		template<class SPRange, class FwdRange> inline 
		typename range_iterator<SPRange>::type find_first_of(SPRange& r, const FwdRange& search_r)
		{
			return i3::find_first_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r));
		}

		template<class SPRange, class FwdRange> inline 
		typename range_iterator<const SPRange>::type find_first_of(const SPRange& r, const FwdRange& search_r)
		{
			return i3::find_first_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r));
		}

		template<class SPRange, class FwdRange, class Pred2> inline 
		typename range_iterator<SPRange>::type find_first_of(SPRange& r, const FwdRange& search_r, Pred2 pr)
		{
			return i3::find_first_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r), pr);
		}

		template<class SPRange, class FwdRange, class Pred2> inline 
		typename range_iterator<const SPRange>::type find_first_of(const SPRange& r, const FwdRange& search_r, Pred2 pr)
		{
			return i3::find_first_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r), pr);
		}

	}

	
}

namespace i3range = i3::range;