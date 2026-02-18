#pragma once

#include "mpl_size_fwd.h"
#include "mpl_int.h"
#include "mpl_iterator_tags.h"

namespace i3
{
	namespace mpl
	{
		struct empty_sequence
		{
			struct tag; 
			struct begin { typedef random_access_iterator_tag category; };    
			typedef begin end;
		};

		template<>
		struct size_impl<empty_sequence::tag>
		{
			template< class Sequence > struct apply : int_<0> {};
		};
	}
}
