#pragma once

#include "mpl_erase_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

#include "mpl_clear.h"
#include "mpl_push_front.h"
#include "mpl_reverse_fold.h"
#include "mpl_iterator_range.h"
#include "mpl_next_prior.h"



namespace i3
{
	namespace mpl
	{
		// 디폴트 구현..
		template<class Tag>
		struct erase_impl
		{
			template<class Seq, class First, class Last>
			struct apply
			{
				typedef typename if_na< Last,typename next<First>::type >::type last_;
        
				// 1st half: [begin, first)
				typedef iterator_range<typename begin<Seq>::type, First > first_half_;

				// 2nd half: [last, end) ... that is, [last + 1, end)
				typedef iterator_range<last_, typename end<Seq>::type > second_half_;

				typedef typename reverse_fold<second_half_, typename clear<Seq>::type, push_front<_,_> >::type half_sequence_;

				typedef typename reverse_fold<first_half_, half_sequence_, push_front<_,_> >::type type;
			};
		};


		template< class Seq = na, class First = na, class Last = na >
		struct erase : erase_impl< typename sequence_tag<Seq>::type >::template apply< Seq,First,Last > {};
	}
}

#define na_spec (3, erase)
#include "aux_/mpl_na_spec.h"
