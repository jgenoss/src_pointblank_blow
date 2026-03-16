#pragma once

#include "mpl_max_element.h"
#include "mpl_nt_less.h"

namespace i3
{
	namespace mpl
	{

		template<class Seq>
		struct nt_max_element : max_element<Seq, nt_less<_,_> > {};

	}
}
