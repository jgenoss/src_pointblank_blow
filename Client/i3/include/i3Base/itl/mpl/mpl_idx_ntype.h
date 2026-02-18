#pragma once

#include "mpl_ntype.h"

namespace i3
{
	namespace mpl
	{
		static const int default_index = -1;

		struct idx_ntype_tag { static const int value = 0; };

		template<int N, class T, T Val>
		struct idx_ntype : ntype<T, Val>
		{
			static const int index = N;
			typedef idx_ntype		type;
			typedef idx_ntype_tag	tag;
		};
			
		namespace aux 
		{
			template< class T > struct has_idx_ntype_tag : is_same<typename T::tag, idx_ntype_tag> {};
		}

		template< class T> 
		struct is_idx_ntype : and_< has_tag<T>, aux::has_idx_ntype_tag<T> > {};
		
		template< class T>
		struct is_not_idx_ntype : not_< is_idx_ntype<T> > {};

	}
}