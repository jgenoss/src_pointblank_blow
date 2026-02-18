#pragma once

#include "mpl_clear_fwd.h"
#include "mpl_sequence_tag.h"

namespace i3
{
	namespace mpl
	{
		template<class Tag> struct clear_impl { template<class Seq> struct apply; };		// 구현없음..
		
		// traits lambda spec
		template<> struct clear_impl<non_sequence_tag> {};

		template<class Seq = na>
		struct clear : clear_impl<typename sequence_tag<Seq>::type >::template apply<Seq> {};
	}
}

#define na_spec (1, clear)
#include "aux_/mpl_na_spec.h"
