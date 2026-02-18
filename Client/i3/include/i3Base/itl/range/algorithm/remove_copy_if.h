#pragma once

#include "../adaptor/generic_inserter.h"
#include "../../algorithm/remove_copy_if.h"

namespace i3
{
	namespace range
	{
		template<class SPRange, class OutIt, class Pred1> inline 
		typename i3::iterator_return<OutIt>::type remove_copy_if(const SPRange& rng, OutIt out, Pred1 pr)
		{
			return i3::remove_copy_if(begin(rng), end(rng), out, pr);
		}

		template<class SPRange, class Seq, class Pred1> inline
		typename i3::non_iterator_return<Seq>::type remove_copy_if(const SPRange& rng, Seq& out, Pred1 pr)
		{
			Seq tmp;
			i3::remove_copy_if( begin(rng), end(rng), i3::generic_inserter(tmp), pr);		//
			i3::copy_or_swap(out, tmp);
			return out;
		}

	}
}

namespace i3range = i3::range;