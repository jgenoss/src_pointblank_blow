#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
////////
#include "mpl_back_inserter.h"
#include "mpl_front_inserter.h"
#include "mpl_push_back.h"
#include "mpl_push_front.h"
#include "mpl_clear.h"
#include "mpl_if.h"
#include "mpl_na.h"
////////

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Seq, class Inserter>
			struct copy_impl : fold< Seq, typename Inserter::state, typename Inserter::operation>{};

			template<class Seq, class Inserter>
			struct reverse_copy_impl : reverse_fold<Seq, typename Inserter::state, typename Inserter::operation>{};
		}

		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na>		// 1,2,na
		struct copy : aux::copy_impl<P1, P2> {};
		
		template<class P1>							// 하나 줄여 특화..
		struct copy<P1, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
									aux::copy_impl<P1, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
									aux::reverse_copy_impl<P1, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na>
		struct reverse_copy : aux::reverse_copy_impl<P1, P2> {};
		
		template<class P1>
		struct reverse_copy<P1, na> : if_<	has_push_back<P1>,
											aux::reverse_copy_impl<P1, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::copy_impl<P1, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		
	}
}

#define na_spec (2, copy)
#include "aux_/mpl_na_spec.h"

#define na_spec (2, reverse_copy)
#include "aux_/mpl_na_spec.h"
