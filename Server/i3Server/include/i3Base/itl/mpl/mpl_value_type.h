#pragma once

#include "mpl_value_type_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_apply_wrap.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class AssociativeSeq = na, class T = na>
		struct value_type : apply_wrap2< value_type_impl< typename sequence_tag<AssociativeSeq>::type >, AssociativeSeq, T >
		{
		};
	}
}

#define na_spec (2, value_type)
#include "aux_/mpl_na_spec.h"
