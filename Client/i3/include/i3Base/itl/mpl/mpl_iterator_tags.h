#pragma once

#include "mpl_int.h"

namespace i3
{
	namespace mpl 
	{
		struct forward_iterator_tag       : int_<0> { typedef forward_iterator_tag type; };
		struct bidirectional_iterator_tag : int_<1> { typedef bidirectional_iterator_tag type; };
		struct random_access_iterator_tag : int_<2> { typedef random_access_iterator_tag type; };
	}
}
