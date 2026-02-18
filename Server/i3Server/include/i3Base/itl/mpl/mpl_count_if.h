#pragma once

#include "mpl_fold.h"
#include "mpl_next_prior.h"
#include "mpl_integral_c.h"
#include "mpl_identity.h"
#include "mpl_if.h"
#include "mpl_apply.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{

			template< class Pred>
			struct next_if
			{
				template< class N, class T>
				struct apply : eval_if< typename apply1<Pred, T>::type, next<N>, identity<N> > {};
			};
			
		}

		template<class Seq = na, class Pred = na>
		struct count_if : fold< Seq, integral_c<unsigned long,0>, protect< aux::next_if<Pred> > >::type {};

	}
}

#define na_spec ( 2, count_if)
#include "aux_/mpl_na_spec.h"
