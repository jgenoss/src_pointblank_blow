#pragma once

#include "mpl_pop_back_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		// 디폴트는 없음..
		template< class Tag >
		struct pop_back_impl {	template< class Seq> struct apply;  };
		
		// traits lambda spec
		template<> struct pop_back_impl<non_sequence_tag> {}; 
		

		template<class Seq = na >
		struct pop_back : pop_back_impl< typename sequence_tag<Seq>::type >::template apply< Seq > {};

	}
}

#define na_spec (1, pop_back)
#include "aux_/mpl_na_spec.h"

