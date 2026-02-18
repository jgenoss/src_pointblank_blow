#pragma once

#include "../../algorithm/lexicographical_compare.h"

namespace i3
{
	namespace range
	{
		template<class FwdRange1, class FwdRange2> inline
		bool lexicographical_compare(const FwdRange1& l, const FwdRange2& r)
		{
			return i3::lexicographical_compare(begin(l), end(l), begin(r), end(r));
		}
		
		template<class FwdRange1, class FwdRange2, class Pred2> inline
		bool lexicographical_compare(const FwdRange1& l, const FwdRange2& r, Pred2 pr)
		{
			return i3::lexicographical_compare(begin(l), end(l), begin(r), end(r), pr);
		}
	}

}

namespace i3range = i3::range;