#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
#include "mpl_protect.h"
#include "aux_/mpl_partition_op.h"

//////
#include "mpl_back_inserter.h"
#include "mpl_front_inserter.h"
#include "mpl_push_back.h"
#include "mpl_push_front.h"
#include "mpl_clear.h"
#include "mpl_if.h"
#include "mpl_na.h"
///////

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template <class Seq, class Pred, class In, class In2, class In1 = typename if_na<In,In2>::type >
			struct stable_partition_impl : fold< Seq , pair< typename In1::state, typename In2::state >, 
														protect< partition_op< Pred, typename In1::operation, typename In2::operation> > >
			{
			};

			template <class Seq, class Pred, class In, class In2, class In1 = typename if_na<In,In2>::type >
			struct reverse_stable_partition_impl : reverse_fold<Seq, pair< typename In1::state, typename In2::state >, 
																	protect< partition_op< Pred, typename In1::operation, typename In2::operation> > >
			{
			};
		}
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>		// 1,2,3,na
		struct stable_partition : aux::stable_partition_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>							// 하나 줄여 특화..
		struct stable_partition<P1, P2, P3, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::stable_partition_impl<P1, P2, P3, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_stable_partition_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>
		struct reverse_stable_partition : aux::reverse_stable_partition_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>
		struct reverse_stable_partition<P1, P2, P3, na> : if_<	has_push_back<P1>,
													aux::reverse_stable_partition_impl<P1, P2, P3, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::stable_partition_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		

	}
}


#define na_spec (4, stable_partition)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_stable_partition)
#include "aux_/mpl_na_spec.h"
