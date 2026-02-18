#pragma once

#define aux_op_name less
#define aux_op_token <
#include "aux_/mpl_comparison_op.h"

/*
namespace i3
{
	namespace mpl
	{
		template<class Tag1, class Tag2>
		struct less_impl : if_c<	( Tag1::value > Tag2::value ), 
									aux::cast2nd_impl< less_impl<Tag1, Tag1>, Tag1, Tag2>,
									aux::cast1st_impl< less_impl<Tag2, Tag2>, Tag1, Tag2> >::type {};

		template<> struct less_impl< na, na>
		{	template<class U1, class U2> struct apply {  typedef apply  type;	static const int value = 0; };	};

		template<class Tag>	struct less_impl< na, Tag>
		{	template<class U1, class U2> struct apply { typedef apply type;		static const int value = 0; };	};

		template<class Tag> struct less_impl< Tag, na>
		{	template<class U1, class U2> struct apply { typedef apply type;		static const int value = 0; };	};

		template<class T> struct less_tag { typedef typename T::tag		type; };
		

		template<class N1 = na, class N2 = na>
		struct less : less_impl< typename less_tag<N1>::type, typename less_tag<N2>::type>::template apply<N1, N2>::type {};
		

		// 실제 구현은 아래임..
		template<>
		struct less_impl< integral_c_tag, integral_c_tag>
		{
			template<class N1, class N2> struct apply : bool_< (N2::value > N1::value) > {};
		};

	}
}

#define na_spec (2, less)
#include "mpl_na_spec.h"
*/
