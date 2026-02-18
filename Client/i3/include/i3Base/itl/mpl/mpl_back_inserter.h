#pragma once

#include "mpl_push_back.h"
#include "mpl_inserter.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq>
		struct back_inserter : inserter< Seq, push_back<> > {};
	}
}
