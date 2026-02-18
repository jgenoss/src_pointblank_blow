
namespace i3
{

	// a_find客 unique 率篮 GCC率苞 促家 去侩...
	template<class forward_iterator> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last)
	{
		if ( first == last ) return last;
		for ( forward_iterator next = first; ++next != last ; first = next )
			if ( *first == *next ) return first;
		return last;
	}

	template<class forward_iterator, class binary_pred> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last, binary_pred pr)
	{
		if ( first == last ) return last;
		for ( forward_iterator next = first; ++next != last ; first = next )
			if ( pr(*first,*next ) ) return first;
		return last;
	}

}
