#pragma once

#include "mpl_front_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

#include "mpl_begin_end.h"
#include "mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		template<class Tag>
		struct front_impl
		{
			template< class Seq > 
			struct apply
			{
				typedef typename begin<Seq>::type iter_;
				typedef typename deref<iter_>::type type;
			};
		};

		// traits lambda spec
		template<> struct front_impl<non_sequence_tag> {};
		
		template<class Seq = na>
		struct front : front_impl< typename sequence_tag<Seq>::type >::template apply< Seq > {};
		
	}
}

#define na_spec (1, front)
#include "aux_/mpl_na_spec.h"
