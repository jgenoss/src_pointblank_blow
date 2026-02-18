#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
#include "mpl_if.h"
#include "mpl_identity.h"
#include "mpl_protect.h"
#include "mpl_lambda.h"
#include "mpl_apply.h"

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
			
			template< class Pred, class InsertOp > 
			struct remove_if_helper
			{
				template< class Seq, class U > 
				struct apply
				{
					typedef typename eval_if<typename apply1<Pred,U>::type, identity<Seq>, apply2<InsertOp,Seq,U> >::type type;
				};
			};

			template< class Seq, class Pred, class Inserter	>
			struct remove_if_impl : fold< Seq,	typename Inserter::state, 
												protect< aux::remove_if_helper<typename lambda<Pred>::type, typename Inserter::operation> >	> {};

			template< class Seq, class Pred, class Inserter>
			struct reverse_remove_if_impl : reverse_fold< Seq,	typename Inserter::state, 
																protect< aux::remove_if_helper<typename lambda<Pred>::type, typename Inserter::operation> > > {};
		}
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct remove_if : aux::remove_if_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct remove_if<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::remove_if_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_remove_if_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_remove_if : aux::reverse_remove_if_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_remove_if<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_remove_if_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::remove_if_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..

	}
}

#define na_spec (3, remove_if)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_remove_if)
#include "aux_/mpl_na_spec.h"
