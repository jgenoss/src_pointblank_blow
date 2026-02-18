#pragma once

#include "mpl_find_of_ntype_index.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq>
		struct find_of_default_index : find_of_ntype_index<Seq, default_index> {};
	}
}

