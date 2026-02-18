
namespace i3
{

	template<class forward_iterator, class T> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val)
	{
		typedef typename iterator_traits<forward_iterator>::difference_type diff;

		diff count = 0;
		i3::distance(f, l, count);

		for (; 0 < count; )
		{	
			diff count2 = count / 2;
			forward_iterator mid = f;
			i3::advance(mid, count2);

			if (*mid < val)
			{	
				f = ++mid;		count -= count2 + 1;
			}
			else if (val < *mid)
				count = count2;	
			else
			{	
				forward_iterator f2 = lower_bound(f, mid, val);
				i3::advance(f, count);
				forward_iterator l2 = upper_bound(++mid, f, val);
				return i3::pair<forward_iterator, forward_iterator>(f2, l2);
			}
		}

		return i3::pair<forward_iterator, forward_iterator>(f, f);	
	}

	template<class forward_iterator, class T, class Pred> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val, Pred pr)
	{
		typedef typename iterator_traits<forward_iterator>::difference_type diff;

		diff count = 0;
		i3::distance(f, l, count);

		for (; 0 < count; )
		{	
			diff count2 = count / 2;
			forward_iterator mid = f;
			i3::advance(mid, count2);

			if (pr(*mid, val) )
			{	
				f = ++mid;		count -= count2 + 1;
			}
			else if (pr(val, *mid) )
				count = count2;	
			else
			{	
				forward_iterator f2 = lower_bound(f, mid, val, pr);
				i3::advance(f, count);
				forward_iterator l2 = upper_bound(++mid, f, val, pr);
				return i3::pair<forward_iterator, forward_iterator>(f2, l2);
			}
		}

		return i3::pair<forward_iterator, forward_iterator>(f, f);	
	}

}

