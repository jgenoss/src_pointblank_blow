
#include "../remove_copy_if.h"

namespace i3
{

	template<class FwdIt, class Pred> inline
	FwdIt	remove_if(FwdIt f, FwdIt l, Pred pr)
	{
		f = find_if(f, l, pr);
		if (f == l) return f;	// empty sequence, all done

		// nonempty sequence, worth doing
		FwdIt f1 = f;
		return remove_copy_if(++f1, l, f, pr);
	}

}
