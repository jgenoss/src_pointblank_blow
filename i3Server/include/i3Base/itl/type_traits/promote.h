#pragma once

#include "integral_promotion.h"
#include "floating_point_promotion.h"

namespace i3
{
	template<class T> 
	struct promote
	{
		typedef typename integral_promotion< typename floating_point_promotion<T>::type>::type type;
	};
}
