#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
#include "mpl_apply.h"
#include "mpl_if.h"
#include "mpl_identity.h"
#include "mpl_protect.h"
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
			template<class Op, class Pred>
			struct copy_if_op
			{
				template< class Seq, class T > 
				struct apply : eval_if<typename apply1<Pred,T>::type, apply2<Op,Seq,T>, identity<Seq> >{};
			};

			template<class Seq, class Pred, class Inserter>
			struct copy_if_impl : fold< Seq, typename Inserter::state, protect< aux::copy_if_op<typename Inserter::operation, Pred> > > { };

			template<class Seq, class Pred, class Inserter>
			struct reverse_copy_if_impl : reverse_fold<Seq, typename Inserter::state, protect< aux::copy_if_op<typename Inserter::operation, Pred> > > {};
		}

		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct copy_if : aux::copy_if_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct copy_if<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::copy_if_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_copy_if_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_copy_if : aux::reverse_copy_if_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_copy_if<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_copy_if_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::copy_if_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		
	}
}

#define na_spec (3, copy_if)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_copy_if)
#include "aux_/mpl_na_spec.h"
