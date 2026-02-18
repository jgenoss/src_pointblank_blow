


namespace i3
{

	template<class FwdIt1, class FwdIt2> inline
	FwdIt1 find_end(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2)
	{
		typename i3::iterator_traits<FwdIt1>::difference_type count1 = 0;
		i3::distance(f1, l1, count1);
		typename i3::iterator_traits<FwdIt2>::difference_type count2 = 0;
		i3::distance(f2, l2, count2);

		FwdIt1 res = l1;

		if (0 < count2)
		{
			for (; count2 <= count1; ++f1, --count1)
			{	
				FwdIt1 mid1 = f1;
				for (FwdIt2 mid2 = f2; ; ++mid1)
				{
					if (!(*mid1 == *mid2)) break;
					else if (++mid2 == l2) { res = f1;	break; }
				}
			}
		}
		return res;
	}

	template<class FwdIt1, class FwdIt2, class Pred2> inline
	FwdIt1 find_end(FwdIt1 f1, FwdIt1 l1, FwdIt2 f2, FwdIt2 l2, Pred2 pr)
	{
		typename i3::iterator_traits<FwdIt1>::difference_type count1 = 0;
		i3::distance(f1, l1, count1);
		typename i3::iterator_traits<FwdIt2>::difference_type count2 = 0;
		i3::distance(f2, l2, count2);

		FwdIt1 res = l1;

		if (0 < count2)
		{
			for (; count2 <= count1; ++f1, --count1)
			{	
				FwdIt1 mid1 = f1;
				for (FwdIt2 mid2 = f2; ; ++mid1)
				{
					if (!pr(*mid1, *mid2)) break;
					else if (++mid2 == l2) { res = f1;	break; }
				}
			}
		}
		return res;
	}

}