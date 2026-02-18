#pragma once

#include "mpl_distance_fwd.h"
#include "mpl_iter_fold.h"
#include "mpl_iterator_range.h"
#include "mpl_long.h"
#include "mpl_next_prior.h"
#include "mpl_tag.h"
#include "mpl_apply_wrap.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template< class Tag > struct distance_impl
		{
			template< class First, class Last > 
			struct apply : iter_fold<iterator_range<First,Last>, mpl::long_<0>, next<> >::type {};
		};		

		template< class First = na, class Last = na >
		struct distance	: distance_impl< typename tag<First>::type >::template apply<First, Last> { };

	}

}

#define na_spec (2, distance)
#include "aux_/mpl_na_spec.h"
