


namespace i3
{
	template<class FwdIt> inline
	FwdIt min_element(FwdIt f, FwdIt l)
	{
		FwdIt found = f;
		if (f != l)
		{
			for (; ++f != l; )
			{
				if ( *f < *found ) found = f;
			}
		}
		return found;
	}

	template<class FwdIt, class Pred2 > inline
	FwdIt min_element(FwdIt f, FwdIt l, Pred2 pr)
	{
		FwdIt found = f;
		if (f != l)
		{
			for (; ++f != l; )
			{
				if (pr(*f, *found) ) found = f;
			}
		}
		return found;
	}

}

