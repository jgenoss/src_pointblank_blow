#pragma once

#include "mpl_idx_ntype.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Tag>
			struct nt_convert_to_int
			{
				template<class NT>
				struct apply {	typedef int_<INT_MAX> type;	};
			};
			
			template<>
			struct nt_convert_to_int<integral_c_tag>
			{
				template<class NT> struct apply { typedef NT type; };
			};
			
			template<>
			struct nt_convert_to_int<idx_ntype_tag>
			{
				template<class NT> struct apply { typedef int_<NT::index> type; };
			};
			
			template<>
			struct nt_convert_to_int<ntype_tag>
			{
				template<class NT> struct apply 
				{	
					typedef typename NT::value_type value_type;
					typedef typename if_< or_<is_integral<value_type>, is_enum<value_type> >, int_<NT::value>, int_<INT_MAX> >::type type;
				};
			};

			template<class NT1, class NT2>
			struct nt_less_impl 
			{
				typedef typename nt_convert_to_int< typename tag<NT1>::type >::template apply<NT1>::type int_1;
				typedef typename nt_convert_to_int< typename tag<NT2>::type >::template apply<NT2>::type int_2;
				typedef typename mpl::less<int_1, int_2>::type				type;
			};
		}
		
		template<class NT1, class NT2>
		struct nt_less : aux::nt_less_impl<NT1, NT2>::type {};
		
	}
}
