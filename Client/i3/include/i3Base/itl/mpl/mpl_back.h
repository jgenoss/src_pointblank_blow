#pragma once

#include "mpl_back_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_begin_end.h"
#include "mpl_next_prior.h"
#include "mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		template< class Tag >
		struct back_impl
		{
			template< class Seq > struct apply
			{
				typedef typename end<Seq>::type end_;
				typedef typename prior<end_>::type last_;
				typedef typename deref<last_>::type type;
			};
		};

		template<> struct back_impl<non_sequence_tag> {};

		template< class Seq = na >
		struct back : back_impl< typename sequence_tag<Seq>::type >::template apply< Seq >{};
	}
}

#define na_spec (1, back)
#include "aux_/mpl_na_spec.h"
