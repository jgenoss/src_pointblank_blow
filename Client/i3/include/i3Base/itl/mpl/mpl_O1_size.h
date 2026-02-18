#pragma once

#include "mpl_O1_size_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_long.h"
#include "mpl_if.h"
#include "../type_traits/has_size.h"


namespace i3
{
	namespace mpl
	{
		namespace aux 
		{
			template< class Seq > struct O1_size_impl : Seq::size {};
		}
		
		template< class Tag >
		struct O1_size_impl
		{
			template< class Seq > struct apply : if_<has_size<Seq>, aux::O1_size_impl<Seq>, long_<-1> >::type {};
		};
		
		template<class Seq = na>
		struct O1_size : O1_size_impl< typename sequence_tag<Seq>::type >::template apply< Seq >{};
	}
}


#define na_spec (1, O1_size)
#include "aux_/mpl_na_spec.h"

