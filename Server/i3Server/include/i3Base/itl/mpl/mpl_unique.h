#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
#include "mpl_if.h"
#include "mpl_and.h"
#include "mpl_identity.h"
#include "mpl_pair.h"
#include "mpl_apply.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			
			template< class Pred, class Operation >
			struct unique_op
			{
				template< class Pair, class T > struct apply
				{
					typedef typename Pair::first seq_;
					typedef typename Pair::second prior_;
					typedef typename eval_if<
						  and_< is_not_na<prior_>, apply2<Pred,prior_,T> >
						, identity<seq_>
						, apply2<Operation,seq_,T>
						>::type new_seq_;

					typedef pair<new_seq_,T> type;
				};
			};

			template< class Seq, class Pred, class Inserter>
			struct unique_impl : first< typename fold<Seq, pair< typename Inserter::state,na >, 
													  protect< aux::unique_op<Pred,typename Inserter::operation> > >::type > {};

			template< class Seq, class Pred, class Inserter>
			struct reverse_unique_impl : first< typename reverse_fold<Seq, pair< typename Inserter::state,na >, 
																		protect< aux::unique_op<Pred,typename Inserter::operation> > >::type >	{};
		}


		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct unique : aux::unique_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct unique<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::unique_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_unique_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_unique : aux::reverse_unique_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_unique<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_unique_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::unique_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		
	}
}


#define na_spec (3, unique)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_unique)
#include "aux_/mpl_na_spec.h"
