#pragma once

#include "mpl_if.h"
#include "mpl_void.h"
#include "../type_traits/has_tag.h"


namespace i3
{
	namespace mpl
	{
		namespace aux 
		{
			template< class T > struct tag_impl {   typedef typename T::tag type; };
		}

		template< class T, class Default = void_ > 
		struct tag : if_< has_tag<T>, aux::tag_impl<T>, Default>::type {};
				
		
	}
}