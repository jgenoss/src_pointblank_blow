#pragma once

#include "../adaptor/generic_inserter.h"
#include "../../algorithm/remove_copy.h"
#include "../iterator.h"
#include "../../utility/copy_or_swap.h"

// 양쪽 버전 모두 리턴값을 반복자로 맞춰놓는다..

namespace i3
{
	namespace range
	{
		template<class SPRange, class OutIt, class Val> inline 
		typename i3::enable_if_iterator<OutIt, OutIt>::type remove_copy( const SPRange& rng, OutIt out, const Val& v)
		{
			return i3::remove_copy(begin(rng), end(rng), out, v);
		}
		
		template<class SPRange, class Seq, class Val> inline
		typename i3::lazy_disable_if_iterator<Seq, i3::range_iterator<Seq> >::type 
		remove_copy(const SPRange& rng, Seq& out, const Val& v)
		{
			Seq tmp;
			i3::remove_copy( begin(rng), end(rng), i3::generic_inserter(tmp), v);		//
			i3::copy_or_swap(out, tmp);
			return out.end();
		}

	}
}

namespace i3range = i3::range;