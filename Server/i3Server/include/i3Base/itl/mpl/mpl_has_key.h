#pragma once

#include "mpl_has_key_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl 
	{
		// no default impl
		template< class Tag > struct has_key_impl
		{
			template< class AssociativeSeq, class Key > struct apply;
		};
		
		// traits lambda spec
		template<> struct has_key_impl<non_sequence_tag> {};
		
		template<class AssociativeSeq = na, class Key = na>
		struct has_key : has_key_impl< typename sequence_tag<AssociativeSeq>::type >::template apply<AssociativeSeq,Key>{ };
	}
}

#define na_spec (2, has_key)
#include "aux_/mpl_na_spec.h"
