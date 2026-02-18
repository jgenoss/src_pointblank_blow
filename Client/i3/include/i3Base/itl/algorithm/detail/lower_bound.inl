
namespace i3
{


	template<class ForwardIterator, class T> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val)
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;

		difference_type Count = 0;
		i3::distance( First, Last, Count );

		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);

			if ( *Mid < Val )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}

		return First;
	}

	template<class ForwardIterator, class T, class Pred> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr )
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;

		difference_type Count = 0;
		i3::distance(First, Last, Count);

		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);

			if ( Pr(*Mid, Val) )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}
		return First;
	}

}

