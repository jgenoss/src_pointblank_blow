#pragma once

#include "mpl_less.h"
#include "mpl_iter_fold.h"
#include "mpl_begin_end.h"
#include "mpl_if.h"
#include "mpl_deref.h"
#include "mpl_apply.h"

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			
			template< class Pred >
			struct select_max
			{
				template< class OldIterator, class Iterator >
				struct apply
				{
					typedef typename apply2<Pred, typename deref<OldIterator>::type, typename deref<Iterator>::type	>::type condition_;
					typedef typename if_<condition_, Iterator, OldIterator>::type type;
				};
			};
		}

		template< class Seq = na, class Pred = less<_,_> >
		struct max_element : iter_fold<Seq, typename begin<Seq>::type, protect< aux::select_max<Pred> > > {};

	}
}

#define na_spec (1, max_element)
#include "aux_/mpl_na_spec.h"
