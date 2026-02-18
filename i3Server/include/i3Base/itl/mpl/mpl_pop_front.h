#pragma once

#include "mpl_pop_front_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		// no default implementation
		template< typename Tag >
		struct pop_front_impl
		{
			template< typename Sequence > struct apply;
		};
		
		// traits lambda spec
		template<> struct pop_front_impl<non_sequence_tag> {};
		
		
		template<class Seq = na>
		struct pop_front : pop_front_impl< typename sequence_tag<Seq>::type >::template apply< Seq >{};
	}
}

#define na_spec (1, pop_front)
#include "aux_/mpl_na_spec.h"
