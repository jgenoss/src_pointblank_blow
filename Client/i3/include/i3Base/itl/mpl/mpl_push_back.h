#pragma once

#include "mpl_push_back_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "../type_traits/is_same.h"

namespace i3
{
	namespace mpl
	{
		
		struct has_push_back_arg {};

		template< class Tag >
		struct push_back_impl
		{
			template< class Seq, class T > 
			struct apply
			{
				compile_assert_msg_type( ( is_same< T, has_push_back_arg >::value ), requested_push_back_specialization_for_sequence_does_not_exist, (Seq) );
			};
		};
		// push_back구현없이 강제로 행한 경우..assert에 걸리게 처리..

		template< class Tag >
		struct has_push_back_impl {	template< class Seq > struct apply : has_type< push_back< Seq, has_push_back_arg > >	{};	};
		
		// traits lambda spec
		template<> struct push_back_impl<non_sequence_tag> {};
		template<> struct has_push_back_impl<non_sequence_tag> {};


		template<class Seq = na, class T = na>
		struct push_back : push_back_impl< typename sequence_tag<Seq>::type >::template apply< Seq,T >{ };

		template<class Seq = na>
		struct has_push_back : has_push_back_impl< typename sequence_tag<Seq>::type >::template apply< Seq >{ };

	}
}


#define na_spec (2, push_back)
#include "aux_/mpl_na_spec.h"

#define na_spec (1, has_push_back)
#include "aux_/mpl_na_spec.h"

// 휴가 종료 후 : 리스트의 push_back 구현 / copy,reverse_copy,reverse등 구현..
