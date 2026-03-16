#pragma once

#include "mpl_iter_apply.h"
#include "../mpl_not.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Pred >
			struct find_if_pred	{ template< class Iterator > struct apply {	typedef not_< aux::iter_apply1<Pred, Iterator> > type;	};	};
		}
	}
}
