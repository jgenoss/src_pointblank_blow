#pragma once

#include "mpl_order_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_if.h"
#include "mpl_long.h"
#include "mpl_has_key.h"

#include "aux_/mpl_ptr_to_ref.h"

namespace i3
{
	namespace mpl
	{

		template<class Seq, class Key>
		struct x_order_impl 
		{
			static const long value = sizeof( Seq::order_by_key_( aux::ptr_to_ref(static_cast<Seq*>(0)), static_cast<identity<Key>*>(0)) );
			typedef long_<value> type;
		};
		
		template< class Tag >
		struct order_impl
		{
			template< class Seq, class Key > 
			struct apply : if_<typename has_key_impl<Tag>::template apply<Seq,Key>, x_order_impl<Seq,Key>, void_>::type {};
		};

		// traits lambda spec
		template<> struct order_impl<non_sequence_tag> {};

		template<class AssociativeSeq = na, class Key = na>
		struct order: order_impl< typename sequence_tag<AssociativeSeq>::type >::template apply<AssociativeSeq,Key> {};

	}
}

#define na_spec (2, order)
#include "aux_/mpl_na_spec.h"

