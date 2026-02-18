#pragma once

#include "ft_arity_limit.h"

#include "../mpl/mpl_size.h"
#include "../mpl/mpl_at.h"

#include "ft_tag.h"
#include "ft_cc_tag.h"
#include "ft_to_class_traits.h"

#include "../type_traits/add_pointer.h"
#include "../type_traits/add_reference.h"

namespace i3
{
	namespace ft
	{
		namespace detail
		{
			template<bits32 Flags, bits32 CCID, std::size_t Size>
			struct synthesize_impl_o {	template<class Seq> struct synthesize_impl_i {};	};

			template<class Seq, bits32 Bits>
			struct synthesize_impl : synthesize_impl_o< decode_flags<Bits>::value, 
														decode_cc_id<Bits>::value, 
														mpl::size<Seq>::value >::template synthesize_impl_i<Seq> {};
		}
		
		template<class Seq, class Tag = null_tag>
		struct synthesize_function : detail::synthesize_impl<Seq, 
			bits<detail::retag_default_cc< tag<nv_dcc_func_tag, Tag> > >::value > {};
		
		template<class Seq, class Tag = null_tag>
		struct synthesize_member_function_pointer : detail::synthesize_impl<Seq,
			bits<detail::retag_default_cc< tag< typename to_class_cv_tag< typename mpl::at_c<Seq,1>::type >::type, nv_dcc_mfp_tag, Tag> > >::value > {};
		
		template<class Seq>
		struct synthesize_member_object_pointer
		{
			typedef typename mpl::at_c<Seq, 0>::type		R;
			typedef typename mpl::at_c<Seq, 1>::type		C;
			typedef R C::* type;
		};	
		
		///
		template<class Seq, class Tag = null_tag>
		struct synthesize_function_pointer : add_pointer< typename synthesize_function<Seq, Tag>::type> {};

		template<class Seq, class Tag = null_tag>
		struct synthesize_function_reference : add_reference< typename synthesize_function<Seq, Tag>::type> {};
		///
	}
}

#include "detail/pp_ft_synthesize_impl.h"
