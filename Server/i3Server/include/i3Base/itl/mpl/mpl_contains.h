#pragma once

#include "mpl_contains_fwd.h"
#include "mpl_sequence_tag.h"

#include "mpl_begin_end.h"
#include "mpl_find.h"
#include "mpl_not.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template <class Tag>
		struct contains_impl
		{
			template< class Seq, class T > 
			struct apply : not_< is_same<typename find<Seq,T>::type, typename end<Seq>::type> > {};
		};

		// traits lambda spec
		template<> struct contains_impl<non_sequence_tag> {};
		
		template<class Seq = na, class T = na>
		struct contains : contains_impl<typename sequence_tag<Seq>::type >::template apply<Seq, T> {};
		
	}
}

#define na_spec (2, contains)
#include "aux_/mpl_na_spec.h"
