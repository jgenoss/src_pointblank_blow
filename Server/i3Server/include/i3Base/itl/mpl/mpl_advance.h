#pragma once



#include "mpl_advance_fwd.h"
#include "mpl_less.h"
#include "mpl_negate.h"
#include "mpl_long.h"
#include "mpl_if.h"
#include "mpl_tag.h"
#include "mpl_apply_wrap.h"
#include "aux_/mpl_advance_forward.h"
#include "aux_/mpl_advance_backward.h"

namespace i3
{
	namespace mpl
	{
		template<class Tag>
		struct advance_impl
		{
			template<class Iterator, class N>
			struct apply 
			{
				typedef typename less<N, long_<0> >::type				negative;
				typedef typename if_<negative, negate<N>, N>::type		offset;
				
				typedef typename if_<negative, aux::advance_backward< offset::value >, aux::advance_forward<offset::value> >::type  tf;
				typedef typename apply_wrap1<tf, Iterator>::type		type;
			};
		};

		template<class Iterator = na, class N = na>
		struct advance : advance_impl<typename tag<Iterator>::type >::template apply<Iterator, N>
		{
		};

		template<class Iterator, long N>
		struct advance_c : advance_impl< typename tag<Iterator>::type >::template apply<Iterator, long_<N> > {};
	}
}


#define na_spec (2, advance)
#include "aux_/mpl_na_spec.h"
