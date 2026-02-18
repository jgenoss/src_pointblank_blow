#include "../copy.h"

namespace i3
{
	template<class InIt1, class InIt2, class OutIt> inline
	OutIt set_union(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, OutIt dest)
	{
		for (; f1 != l1 && f2 != l2; )
		{
			if (*f1 < *f2) *dest++ = *f1, ++f1;
			else if (*f2 < *f1) *dest++ = *f2, ++f2;
			else *dest++ = *f1, ++f1, ++f2;
		}

		dest = i3::copy(f1, l1, dest);
		return i3::copy(f2, l2, dest);
	}

	template<class InIt1, class InIt2, class OutIt, class Pred2> inline
	OutIt set_union(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, OutIt dest, Pred2 pr)
	{
		for (; f1 != l1 && f2 != l2; )
		{
			if (pr(*f1, *f2)) *dest++ = *f1, ++f1;
			else if (pr(*f2, *f1)) *dest++ = *f2, ++f2;
			else *dest++ = *f1, ++f1, ++f2;
		}

		dest =  i3::copy(f1, l1, dest);
		return  i3::copy(f2, l2, dest);
	}

}
