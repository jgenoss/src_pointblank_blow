#pragma once

#include "mpl_partition.h"
#include "mpl_copy.h"
#include "mpl_vector.h"
#include "mpl_back_inserter.h"
#include "mpl_front_inserter.h"
#include "mpl_iterator_range.h"
#include "mpl_joint_view.h"
#include "mpl_single_view.h"
#include "mpl_begin_end.h"
#include "mpl_empty.h"
#include "mpl_deref.h"
#include "mpl_if.h"
#include "mpl_apply.h"
#include "mpl_identity.h"
#include "mpl_less.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Seq, class Pred >
			struct quick_sort;

			template< class Pred, class Pivot >
			struct quick_sort_pred
			{
				template< class T > struct apply
				{
					typedef typename apply2<Pred,T,Pivot>::type type;
				};
			};

			template< class Seq, class Pred>
			struct quick_sort_impl
			{
				typedef typename begin<Seq>::type pivot;
				typedef typename partition< iterator_range< typename next<pivot>::type, typename end<Seq>::type>, 
											protect< aux::quick_sort_pred< Pred, typename deref<pivot>::type > >, 
											back_inserter< vector<> >, 
											back_inserter< vector<> > >::type partitioned;

				typedef typename quick_sort< typename partitioned::first, Pred >::type part1;
				typedef typename quick_sort< typename partitioned::second, Pred >::type part2;

				typedef joint_view< joint_view< part1, single_view< typename deref<pivot>::type > > , part2	> type;
			};

			template< class Seq, class Pred>
			struct quick_sort : eval_if< empty<Seq>, identity<Seq>, quick_sort_impl<Seq,Pred> > {};
			

			template < class Seq, class Pred, class In>
			struct sort_impl
			{
				typedef typename quick_sort< Seq, typename if_na<Pred,less<> >::type>::type result_;
				typedef typename copy<result_,In>::type type;
			};

			template < class Seq, class Pred, class In>
			struct reverse_sort_impl
			{
				typedef typename quick_sort< Seq, typename if_na<Pred,less<> >::type>::type result_;
				typedef typename reverse_copy<result_,In>::type type;
			};
		}
		
		// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct sort : aux::sort_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct sort<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::sort_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_sort_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_sort : aux::reverse_sort_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_sort<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_sort_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::sort_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		

	}
}

#define na_spec (3, sort)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_sort)
#include "aux_/mpl_na_spec.h"
