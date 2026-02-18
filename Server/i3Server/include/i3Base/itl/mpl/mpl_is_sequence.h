#pragma once

#include "mpl_not.h"
#include "mpl_begin_end.h"
#include "mpl_void.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template< class T = na >
		struct is_sequence : not_< is_same< typename begin<T>::type, void_ > > {};
	}
}

#define na_spec (1, is_sequence)
#include "aux_/mpl_na_spec.h"
