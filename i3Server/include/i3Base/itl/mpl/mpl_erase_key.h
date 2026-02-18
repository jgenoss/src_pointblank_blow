#pragma once

#include "mpl_erase_key_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Tag>
		struct erase_key_impl
		{
			template<class Seq, class Key> struct apply;			// 구현없음..
		};
		
		// traits lambda spec
		template<> struct erase_key_impl<non_sequence_tag> {};
		
		template<class Seq = na, class Key = na>
		struct erase_key : erase_key_impl< typename sequence_tag<Seq>::type >::template apply<Seq, Key> {};
	}
}

#define na_spec (2, erase_key)
#include "aux_/mpl_na_spec.h"
