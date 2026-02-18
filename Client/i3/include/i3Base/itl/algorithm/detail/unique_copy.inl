
namespace i3
{
	namespace detail
	{
		
		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::input_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !(val == *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}
		// outputiter는 inputiter와 동일하게..
		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::output_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !(val == *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}

		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::forward_iterator_tag)
		{
			for ( *dest = *first ; ++first != last ; )
				if ( !(*dest == *first) )
					*++dest = *first;
			return ++dest;
		}

		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::input_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !pr(val, *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}
		// outputiter는 inputiter와 동일하게..
		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::output_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !pr(val, *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}

		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::forward_iterator_tag)
		{
			for ( *dest = *first ; ++first != last ; )
				if ( !pr(*dest, *first) )
					*++dest = *first;
			return ++dest;
		}
	
	}
	
	template<class  input_iterator, class output_iterator> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest)
	{
		return ( first == last ) ? dest : detail::unique_copy_impl(first, last, dest, typename i3::iterator_traits<input_iterator>::iterator_category() );
	}

	template<class  input_iterator, class output_iterator, class binary_pred> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest, binary_pred pr)
	{
		return ( first == last ) ? dest : detail::unique_copy_impl(first, last, dest, pr, typename i3::iterator_traits<input_iterator>::iterator_category() );
	}

}
