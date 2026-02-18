
#include "../remove_copy.h"

namespace i3
{

	template<class FwdIt, class T> inline
	FwdIt	remove(FwdIt f, FwdIt l, const T& val)
	{
		f = find(f, l, val);
		if (f == l) return f;	// empty sequence, all done

		// nonempty sequence, worth doing
		FwdIt f1 = f;
		return remove_copy(++f1, l, f, val);
	}

}
