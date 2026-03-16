#pragma once

#include "mpl_integral_c.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		template<class Tag> struct negate_impl;
		template<class T> struct negate_tag { typedef typename T::tag type; };
		
		template<class N = na>
		struct negate : negate_impl< typename negate_tag<N>::type>::template apply<N>::type{ };

		template<>
		struct negate_impl<integral_c_tag>
		{
			template<class N> struct apply : integral_c< typename N::value_type, (-N::value) > {};
		};
				
	}
}

#define na_spec (1, negate)
#include "aux_/mpl_na_spec.h"

