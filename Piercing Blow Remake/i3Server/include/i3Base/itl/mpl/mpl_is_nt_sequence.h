#pragma once

#include "mpl_is_sequence.h"
#include "mpl_empty.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Seq>
			struct is_nt_sequence_impl_impl
			{
				typedef typename deref<typename begin<Seq>::type>::type  nt;
				typedef mpl::or_< is_idx_ntype<nt>, is_ntype<nt> >	type;
			};
			
			template<class Seq>
			struct is_nt_sequence_impl : is_nt_sequence_impl_impl<Seq>::type {};
			
			template<class Seq>
			struct is_ntype_sequence_impl : is_ntype< typename deref<typename begin<Seq>::type>::type > {};

			template<class Seq>
			struct is_idx_ntype_sequence_impl : is_idx_ntype< typename deref<typename begin<Seq>::type>::type > {};
		}

		template<class Seq>
		struct is_nt_sequence : and_<is_sequence<Seq>, not_< empty<Seq> >, aux::is_nt_sequence_impl<Seq> > {};
		
		template<class Seq>
		struct is_ntype_sequence : and_<is_sequence<Seq>, not_< empty<Seq> >, aux::is_ntype_sequence_impl<Seq> > {};

		template<class Seq>
		struct is_idx_ntype_sequence : and_<is_sequence<Seq>, not_< empty<Seq> >, aux::is_idx_ntype_sequence_impl<Seq> > {};


	}
}