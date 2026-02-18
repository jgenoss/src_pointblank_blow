#pragma once

#include "mpl_fold.h"
#include "mpl_empty_base.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Types, class Node, class Root = empty_base>
		struct inherit_linearly : fold<Types, Root, Node> {};
	}
}

#define na_spec (2, inherit_linearly)
#include "aux_/mpl_na_spec.h"

