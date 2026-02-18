#pragma once

#include "mpl_count_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_count_if.h"
#include "mpl_same_as.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template< class Tag > 
		struct count_impl
		{
			template< class Seq, class T > 
			struct apply : count_if< Seq,same_as<T> >{};
		};
		
		// traits lambda spec
		template<> struct count_impl<non_sequence_tag> {};

		template<class Seq = na, class T = na >
		struct count : count_impl<typename sequence_tag<Seq>::type >::template apply<Seq, T> {};

	}
}

#define na_spec (2, count)
#include "aux_/mpl_na_spec.h"
