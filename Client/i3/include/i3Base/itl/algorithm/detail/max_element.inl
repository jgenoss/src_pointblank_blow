

namespace i3
{
	template<class FwdIt> inline
	FwdIt max_element(FwdIt f, FwdIt l)
	{
		FwdIt found = f;
		if (f != l)
		{
			for (; ++f != l; )
			{
				if (*found < *f) found = f;
			}
		}
		return found;
	}

	template<class FwdIt, class Pred2 > inline
	FwdIt max_element(FwdIt f, FwdIt l, Pred2 pr)
	{
		FwdIt found = f;
		if (f != l)
		{
			for (; ++f != l; )
			{
				if (pr(*found, *f)) found = f;
			}
		}
		return found;
	}

}
