#pragma once

#include "mpl_is_sequence.h"
#include "mpl_single_view.h"
#include "mpl_if.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		template<class T = na>
		struct as_sequence : if_< is_sequence<T>, T, single_view<T> >{};
	}
}

#define na_spec (1, as_sequence)
#include "aux_/mpl_na_spec.h"
