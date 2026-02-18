#pragma once

#include "mpl_empty_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_begin_end.h"

namespace i3
{
	namespace mpl 
	{
		template<class Tag>
		struct empty_impl 
		{
			template<class Seq> struct apply : is_same< typename begin<Seq>::type, typename end<Seq>::type> {};
		};
		
		// traits lambda spec
		template<> struct empty_impl<non_sequence_tag> {};
		
		template<class Seq = na>
		struct empty : empty_impl< typename sequence_tag<Seq>::type >::template apply<Seq> {};
	}
}

#define na_spec (1, empty)
#include "aux_/mpl_na_spec.h"
