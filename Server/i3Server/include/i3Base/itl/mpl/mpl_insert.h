#pragma once

#include "mpl_insert_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

#include "mpl_reverse_fold.h"
#include "mpl_iterator_range.h"
#include "mpl_clear.h"
#include "mpl_push_front.h"


namespace i3
{
	namespace mpl 
	{
		// 디폴트 구현..		
		template< class Tag >
		struct insert_impl
		{
			template<class Seq, class Pos, class T>
			struct apply
			{
				typedef iterator_range<typename begin<Seq>::type, Pos> first_half_;
				typedef iterator_range<Pos, typename end<Seq>::type> second_half_;
				typedef typename reverse_fold< second_half_, typename clear<Seq>::type, push_front<_,_>	>::type half_sequence_;
				typedef typename reverse_fold< first_half_, typename push_front<half_sequence_,T>::type, push_front<_,_> >::type type;
			};
		};
		
		// traits lambda spec
		template<> struct insert_impl<non_sequence_tag> {};

		template<class Seq = na, class Pos_or_T = na, class T = na>
		struct insert : insert_impl< typename sequence_tag<Seq>::type >::template apply< Seq,Pos_or_T,T >{};
	}
}

#define na_spec (3, insert)
#include "aux_/mpl_na_spec.h"
