
#include "../../iter_swap.h"

namespace i3
{
	template<class FwdIt1, class FwdIt2> inline
	FwdIt2 swap_ranges(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2)
	{
		for (; f1 != l1; ++f1, ++f2)
			i3::iter_swap(f1, f2);
		return f2;
	}
}
