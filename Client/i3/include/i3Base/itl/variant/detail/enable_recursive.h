#pragma once

#include "enable_recursive_fwd.h"
#include "substitute.h"

#include "../recursive_wrapper.h"


namespace i3
{
	namespace detail
	{
		namespace variant
		{
			
			template<class T, class RecursiveVariant, class NoWrapper>
			struct enable_recursive : substitute<T, RecursiveVariant, recursive_variant_>
			{
			};

			template<class T, class RecursiveVariant>
			struct enable_recursive<T, RecursiveVariant, mpl::false_>
			{
				typedef typename substitute<T, RecursiveVariant, recursive_variant_>::type t_;

				typedef typename mpl::if_< mpl::or_< is_same<t_, T>, is_reference<t_>, is_pointer<t_> >,
					t_, i3::recursive_wrapper<t_> >::type type;
			};

			template<class RecursiveVariant, class NoWrapper>
			struct quoted_enable_recursive
			{
				template<class T>
				struct apply : enable_recursive<T, RecursiveVariant, NoWrapper>
				{

				};
			};

		}
	}
}