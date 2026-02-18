#pragma once

#include "mpl_push_front.h"
#include "mpl_inserter.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq> struct front_inserter : inserter<Seq, push_front<> > {};
	}
}
