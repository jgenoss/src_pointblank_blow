

namespace i3
{
	namespace detail
	{
		template<class FwdIt, class Diff, class T> inline
		FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, i3::forward_iterator_tag )		
		{
			if (count <= 0) return f;

			for (; f != l; ++f)
			{
				if (*f == val)
				{	
					FwdIt mid  = f;

					for (Diff count1 = count; ; )
					{
						if (--count1 == 0)	return f;	
						else if (++mid == l) return l;	
						else if (!(*mid == val)) break;	
					}
					f = mid;	
				}
			}

			return l;
		}

		template<class FwdIt, class Diff, class T> inline
		FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, i3::random_access_iterator_tag )		
		{
			if (count <= 0) return f;

			FwdIt old_f = f;
			for (Diff inc = 0; count <= l - old_f; )
			{	
				f = old_f + inc;

				if (*f == val)
				{	
					Diff count1 = count;	FwdIt mid = f;

					for (; old_f != f && f[-1] == val; --f)	--count1;	

					if (count1 <= l - mid)
					{
						for ( ; ; )	
						{
							if (--count1 == 0)	return f;	
							else if (!(*++mid == val)) break;	
						}
					}
					old_f = ++mid;	inc = 0;
				}
				else
				{	
					old_f = f + 1;	inc = count - 1;
				}
			}
			return l;
		}

		template<class FwdIt, class Diff, class T, class Pred2> inline
		FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, Pred2 pr, i3::forward_iterator_tag )		
		{
			if (count <= 0) return f;

			for (; f != l; ++f)
			{
				if ( pr(*f, val) )
				{	
					FwdIt mid  = f;

					for (Diff count1 = count; ; )
					{
						if (--count1 == 0)	return f;	
						else if (++mid == l) return l;	
						else if (!pr(*mid, val)) break;	
					}
					f = mid;	
				}
			}
			return l;
		}
		
		template<class FwdIt, class Diff, class T, class Pred2> inline
		FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, Pred2 pr, i3::random_access_iterator_tag )		
		{
			if (count <= 0) return f;

			FwdIt old_f = f;
			for (; count <= l - old_f; )
			{	
				if ( pr(*f, val) )
				{	
					Diff count1 = count;	FwdIt mid = f;

					for (; old_f != f && pr(f[-1], val) ; --f)	--count1;	

					if (count1 <= l - mid)
					{
						for ( ; ; )	
						{
							if (--count1 == 0)	return f;	
							else if (!pr(*++mid, val)) break;	
						}
					}
					old_f = ++mid;	f = old_f;
				}
				else
				{	
					old_f = f + 1;	f += count;
				}
			}
			return l;
		}
	
	}

	template<class FwdIt, class Diff, class T> inline
	FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val)
	{
		return detail::search_n(f, l, count, val, typename i3::iterator_traits<FwdIt>::iterator_category() );
	}

	template<class FwdIt, class Diff, class T, class Pred2> inline
	FwdIt search_n(FwdIt f, FwdIt l, Diff count, const T& val, Pred2 pr)
	{
		return detail::search_n(f, l, count, val, pr, typename i3::iterator_traits<FwdIt>::iterator_category() );
	}


}