#pragma once

#include "mpl_transform.h"
#include "mpl_apply.h"
#include "mpl_if.h"

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
			
			template< class Pred, class T >
			struct replace_if_op
			{
				template< class U > 
				struct apply : if_< typename apply1<Pred,U>::type, T, U	>{};
			};

			template< class Seq, class Pred, class T, class Inserter>
			struct replace_if_impl : transform1_impl< Seq, protect< aux::replace_if_op<Pred,T> >, Inserter> {};

			template< class Seq, class Pred, class T, class Inserter>
			struct reverse_replace_if_impl : reverse_transform1_impl< Seq, protect< aux::replace_if_op<Pred,T> >, Inserter>{};
		}		
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>		// 1,2,3,na
		struct replace_if : aux::replace_if_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>							// 하나 줄여 특화..
		struct replace_if<P1, P2, P3, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::replace_if_impl<P1, P2, P3, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_replace_if_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>
		struct reverse_replace_if : aux::reverse_replace_if_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>
		struct reverse_replace_if<P1, P2, P3, na> : if_<	has_push_back<P1>,
													aux::reverse_replace_if_impl<P1, P2, P3, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::replace_if_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
				
	}
}

#define na_spec (4, replace_if)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_replace_if)
#include "aux_/mpl_na_spec.h"
