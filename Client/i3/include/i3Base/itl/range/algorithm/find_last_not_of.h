#pragma once


#include "../../algorithm/find_last_not_of.h"

namespace i3
{
	// SPRange = SinglePassRange
	namespace range
	{
		template<class BidRange, class FwdRange> inline 
		typename range_iterator<BidRange>::type find_last_not_of(BidRange& r, const FwdRange& search_r)
		{
			return i3::find_last_not_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r));
		}

		template<class BidRange, class FwdRange> inline 
		typename range_iterator<const BidRange>::type find_last_not_of(const BidRange& r, const FwdRange& search_r)
		{
			return i3::find_last_not_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r));
		}

		template<class BidRange, class FwdRange, class Pred2> inline 
		typename range_iterator<BidRange>::type find_last_not_of(BidRange& r, const FwdRange& search_r, Pred2 pr)
		{
			return i3::find_last_not_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r), pr);
		}

		template<class BidRange, class FwdRange, class Pred2> inline 
		typename range_iterator<const BidRange>::type find_last_not_of(const BidRange& r, const FwdRange& search_r, Pred2 pr)
		{
			return i3::find_last_not_of(i3::begin(r), i3::end(r), i3::begin(search_r), i3::end(search_r), pr);
		}

	}


}

namespace i3range = i3::range;
