#pragma once

#include "mpl_insert_range_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

#include "mpl_copy.h"
#include "mpl_clear.h"
#include "mpl_front_inserter.h"
#include "mpl_joint_view.h"
#include "mpl_iterator_range.h"
#include "aux_/mpl_iter_push_front.h"

namespace i3
{
	namespace mpl
	{
		
		template< class Tag >
		struct insert_range_impl
		{
			template<class Seq, class Pos, class Range>
			struct apply : reverse_copy< joint_view	< iterator_range<typename begin<Seq>::type,Pos>, 
													 joint_view<Range, iterator_range<Pos,typename end<Seq>::type> >
													>
										, front_inserter< typename clear<Seq>::type > >
			{
			};
		};
		
		// traits lambda spec
		template<> struct insert_range_impl<non_sequence_tag> {};
		
		template<class Seq = na, class Pos = na, class Range = na>
		struct insert_range : insert_range_impl< typename sequence_tag<Seq>::type >::template apply< Seq,Pos,Range >{};

	}
}

#define na_spec (3, insert_range)
#include "aux_/mpl_na_spec.h"
