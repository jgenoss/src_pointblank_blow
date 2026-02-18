#pragma once

#include "../type_traits/has_tag.h"

namespace i3
{
	namespace mpl
	{
		struct nt {};
		struct ntype_tag { static const int value = 0; };
		
		template <class T, T Val>
		struct ntype : nt
		{
			static const T		value;
			typedef ntype		type;
			typedef T			value_type;
			typedef ntype_tag	tag;
		};
		
		template<class T, T Val>
		const T ntype<T,Val>::value = Val;

		namespace aux 
		{
			template< class T > struct has_ntype_tag : is_same<typename T::tag, ntype_tag> {};
		}

		template< class T> 
		struct is_ntype : and_< has_tag<T>, aux::has_ntype_tag<T> > {};
		
		template< class T>
		struct is_not_ntype : not_< is_ntype<T> > {};
		
		template<class T>
		struct is_nt : is_base_and_derived<nt, T> {};
		
		template<class T>
		struct is_not_nt : not_< is_nt<T> > {};

	}

}

