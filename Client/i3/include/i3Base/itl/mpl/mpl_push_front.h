#pragma once

#include "mpl_push_front_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		struct has_push_front_arg {};

		template< class Tag >
		struct push_front_impl
		{
			template< class Seq, class T > 
			struct apply
			{
				// push_back구현없이 강제로 행한 경우..assert에 걸리게 처리..
				compile_assert_msg_type( ( is_same< T, has_push_front_arg >::value ), requested_push_front_specialization_for_sequence_does_not_exist, (Seq));
			};
		};

		template< class Tag >
		struct has_push_front_impl {	template< class Seq > struct apply : has_type< push_front< Seq, has_push_front_arg > >	{};	};
		
		// traits lambda spec
		template<> struct push_front_impl<non_sequence_tag> {};
		template<> struct has_push_front_impl<non_sequence_tag> {};


		template<class Seq = na, class T = na>
		struct push_front : push_front_impl< typename sequence_tag<Seq>::type >::template apply<Seq, T> {};

		template<class Seq = na>
		struct has_push_front : has_push_front_impl< typename sequence_tag<Seq>::type >::template apply<Seq> {};
	}
}


#define na_spec (2, push_front)
#include "aux_/mpl_na_spec.h"

#define na_spec (1, has_push_front)
#include "aux_/mpl_na_spec.h"


