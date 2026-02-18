#pragma once

#include "mpl_nt_max_element.h"
#include "mpl_size_t.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class NTSeq>
			struct nt_capacity_idx_ntype
			{
				typedef typename deref<typename nt_max_element<NTSeq>::type>::type	deref_type;
				static const std::size_t											value = deref_type::index + 1;
				typedef mpl::size_t< value >										type;
			};
		}

		template<class NTSeq>
		struct nt_capacity : eval_if<	is_idx_ntype_sequence<NTSeq>, 
										aux::nt_capacity_idx_ntype<NTSeq>, 
										size<NTSeq> > {};
	}
}