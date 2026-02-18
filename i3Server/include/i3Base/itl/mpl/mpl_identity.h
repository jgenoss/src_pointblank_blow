#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class T = na> struct identity { typedef T type; };
		template<class T = na> struct make_identity { typedef identity<T> type; };
	}

	using mpl::identity;				// identity를 i3에 노출하기로 결정...
}


#define na_spec (1, identity)
#include "aux_/mpl_na_spec.h"
#define na_spec (1, make_identity)
#include "aux_/mpl_na_spec.h"
