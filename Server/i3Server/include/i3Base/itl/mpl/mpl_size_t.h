#pragma once

#include "mpl_size_t_fwd.h"
#include "mpl_integral_c_tag.h"


namespace i3
{
	namespace mpl
	{
		
		template< std::size_t N >
		struct size_t
		{
			static	const std::size_t		value = N;
			typedef size_t					type;
			typedef std::size_t				value_type;
			typedef integral_c_tag			tag;
			typedef size_t< static_cast<std::size_t>(value + 1) >	next;
			typedef size_t< static_cast<std::size_t>(value - 1) >	prior;
	
			operator std::size_t () const { return this->value; } 
		};
		
	}
}
