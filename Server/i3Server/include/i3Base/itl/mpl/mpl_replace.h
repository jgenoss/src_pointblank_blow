#pragma once

#include "mpl_replace_if.h"
#include "mpl_same_as.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Seq, class OldType, class NewType, class Inserter>
			struct replace_impl : replace_if_impl< Seq, same_as<OldType>, NewType, Inserter > {};

			template< class Seq, class OldType, class NewType, class Inserter>
			struct reverse_replace_impl : reverse_replace_if_impl< Seq, same_as<OldType>, NewType, Inserter > {};
		}

		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>		// 1,2,3,na
		struct replace : aux::replace_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>							// 하나 줄여 특화..
		struct replace<P1, P2, P3, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::replace_impl<P1, P2, P3, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_replace_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>
		struct reverse_replace : aux::reverse_replace_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>
		struct reverse_replace<P1, P2, P3, na> : if_<	has_push_back<P1>,
													aux::reverse_replace_impl<P1, P2, P3, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::replace_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..

	}
}


#define na_spec (4, replace)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_replace)
#include "aux_/mpl_na_spec.h"
