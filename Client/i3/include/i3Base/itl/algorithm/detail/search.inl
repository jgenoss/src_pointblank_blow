

namespace i3
{

	template<class ForwardIterator1, class ForwardIterator2> inline
	ForwardIterator1 search(ForwardIterator1 f1, ForwardIterator1 l1, ForwardIterator2 f2, ForwardIterator2 l2)
	{
		typename i3::iterator_traits<ForwardIterator1>::difference_type	Count1 = 0;
		i3::distance(f1, l1, Count1);
		typename i3::iterator_traits<ForwardIterator2>::difference_type	Count2 = 0;
		i3::distance(f2, l2, Count2);

		for (; Count2 <= Count1; ++f1, --Count1)
		{	
			ForwardIterator1 mid1 = f1;
			for (ForwardIterator2 mid2 = f2; ; ++mid1, ++mid2)
			{
				if (mid2 == l2) return f1;
				else if ( !(*mid1 == *mid2) ) break;
			}
		}
		return l1;
	}

	template<class ForwardIterator1, class ForwardIterator2, class Pred2> inline
	ForwardIterator1 search(ForwardIterator1 f1, ForwardIterator1 l1, ForwardIterator2 f2, ForwardIterator2 l2, Pred2 pr)
	{
		typename i3::iterator_traits<ForwardIterator1>::difference_type	Count1 = 0;
		i3::distance(f1, l1, Count1);
		typename i3::iterator_traits<ForwardIterator2>::difference_type	Count2 = 0;
		i3::distance(f2, l2, Count2);

		for (; Count2 <= Count1; ++f1, --Count1)
		{	
			ForwardIterator1 mid1 = f1;
			for (ForwardIterator2 mid2 = f2; ; ++mid1, ++mid2)
			{
				if (mid2 == l2) return f1;
				else if ( !pr(*mid1, *mid2) ) break;
			}
		}
		return l1;
	}


}