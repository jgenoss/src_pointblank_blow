#pragma once

#include "mpl_integral_c.h"
#include "mpl_range_c_core.h"

namespace i3
{
	namespace mpl
	{
		template<class T, T Start, T Finish>
		struct range_c
		{
			typedef aux::half_open_range_tag tag;
			typedef T value_type;
			typedef range_c type;

			typedef integral_c<T,Start> start;
			typedef integral_c<T,Finish> finish;

			typedef r_iter<start> begin;
			typedef r_iter<finish> end;
		};
	}
}
