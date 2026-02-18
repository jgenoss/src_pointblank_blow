#pragma once

#include "mpl_remove_if.h"
#include "mpl_same_as.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Seq, class T, class Inserter>
			struct remove_impl : remove_if_impl< Seq, same_as<T>, Inserter >{};

			template<class Seq, class T, class Inserter>
			struct reverse_remove_impl : reverse_remove_if_impl< Seq, same_as<T>, Inserter > {};
		}
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct remove : aux::remove_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct remove<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::remove_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_remove_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_remove : aux::reverse_remove_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_remove<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_remove_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::remove_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		
	}
}

#define na_spec (3, remove)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_remove)
#include "aux_/mpl_na_spec.h"
