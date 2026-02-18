

namespace i3
{
	namespace detail
	{
		template<class FwdIt, class Diff, class Pred1> inline
		FwdIt search_n_if(FwdIt f, FwdIt l, Diff count,  Pred1 pr, i3::forward_iterator_tag )		
		{
			if (count <= 0) return f;

			for (; f != l; ++f)
			{
				if ( pr(*f) )
				{	
					FwdIt mid  = f;

					for (Diff count1 = count; ; )
					{
						if (--count1 == 0)	return f;	
						else if (++mid == l) return l;	
						else if (!pr(*mid)) break;	
					}
					f = mid;	
				}
			}
			return l;
		}

		template<class FwdIt, class Diff, class Pred1> inline
		FwdIt search_n_if(FwdIt f, FwdIt l, Diff count, Pred1 pr, i3::random_access_iterator_tag )		
		{
			if (count <= 0) return f;

			FwdIt old_f = f;
			for (; count <= l - old_f; )
			{	
				if ( pr(*f) )
				{	
					Diff count1 = count;	FwdIt mid = f;

					for (; old_f != f && pr(f[-1]) ; --f)	--count1;	

					if (count1 <= l - mid)
					{
						for ( ; ; )	
						{
							if (--count1 == 0)	return f;	
							else if (!pr(*++mid)) break;	
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

	// 다음은 비표준..
	template<class FwdIt, class Diff, class Pred1> inline
	FwdIt search_n_if(FwdIt f, FwdIt l, Diff count, Pred1 pr)
	{
		return detail::search_n_if(f, l, count, pr, typename i3::iterator_traits<FwdIt>::iterator_category() );
	}


}
