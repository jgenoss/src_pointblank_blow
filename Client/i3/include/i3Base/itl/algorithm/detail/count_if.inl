
namespace i3
{

	template<class InputIterator, class Pred> inline
	typename iterator_traits<InputIterator>::difference_type count_if( InputIterator f, InputIterator l, Pred pr)
	{
		typename iterator_traits<InputIterator>::difference_type c = 0;
		for (; f != l; ++f)
			if ( pr(*f) ) ++c;
		return c;
	}

}
