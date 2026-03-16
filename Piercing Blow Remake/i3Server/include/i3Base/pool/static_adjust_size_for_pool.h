#pragma once

#include "enable_pool_dbg.h"
#include "static_calculate_pool_data_size_dbg.h"
#include "../itl/mpl/mpl_if.h"
#include "../itl/type_traits/alignment_of.h"

namespace i3
{
	template<class T>
	struct static_adjust_size_for_pool  
	{
		typedef typename mpl::if_c<	enable_pool_dbg<T>::value,
									static_calculate_pool_data_size_dbg<sizeof(T), alignment_of<T>::value >,
									static_calculate_adjusted_size<sizeof(T)> >::type type;
		static const std::size_t value = type::value;
	};

}
