#pragma once

#include "mpl_stable_partition.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template <class Seq, class Pred, class In1, class In2>
			struct partition_impl : stable_partition_impl<Seq,Pred,In1,In2> {};

			template <class Seq, class Pred, class In1, class In2>
			struct reverse_partition_impl : reverse_stable_partition_impl<Seq,Pred,In1,In2> {};
		}
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>		// 1,2,3,na
		struct partition : aux::partition_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>							// 하나 줄여 특화..
		struct partition<P1, P2, P3, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::partition_impl<P1, P2, P3, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_partition_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>
		struct reverse_partition : aux::reverse_partition_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>
		struct reverse_partition<P1, P2, P3, na> : if_<	has_push_back<P1>,
													aux::reverse_partition_impl<P1, P2, P3, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::partition_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
				
	}
}


#define na_spec (4, partition)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_partition)
#include "aux_/mpl_na_spec.h"
