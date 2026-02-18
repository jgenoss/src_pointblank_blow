#pragma once

#include "mpl_find_if.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class Pred= na>
		struct contains_if : not_< is_same< typename find_if<Seq, Pred>::type, typename end<Seq>::type> > {};
	}
}


#define na_spec (2, contains_if)
#include "aux_/mpl_na_spec.h"
