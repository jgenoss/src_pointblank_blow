#pragma once

#include "aux_/mpl_single_element_iter.h"
#include "mpl_iterator_range.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class T = na>
		struct single_view : iterator_range<aux::sel_iter<T, 0>, aux::sel_iter<T, 1> > {};
	}
}


#define na_spec (1, single_view)
#include "aux_/mpl_na_spec.h"
