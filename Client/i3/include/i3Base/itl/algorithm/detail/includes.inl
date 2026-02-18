

namespace i3
{
	
	template<class InIt1, class InIt2> inline
	bool includes(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2)
	{
		for (; f1 != l1 && f2 != l2; )
		{
			if (*f2 < *f1) return false;
			else if (*f1 < *f2) ++f1;
			else ++f1, ++f2;
		}
		return (f2 == l2);
	}

	template<class InIt1, class InIt2, class Pred2> inline
	bool includes(InIt1 f1, InIt1 l1, InIt2 f2, InIt2 l2, Pred2 pr)
	{
		for (; f1 != l1 && f2 != l2; )
		{
			if ( pr(*f2, *f1) ) return false;
			else if (pr(*f1, *f2)) ++f1;
			else ++f1, ++f2;
		}
		return (f2 == l2);
	}
	
}
