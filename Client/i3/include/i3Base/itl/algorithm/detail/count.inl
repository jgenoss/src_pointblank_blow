

namespace i3
{

	template<class InputIterator, class T> inline
	typename iterator_traits<InputIterator>::difference_type count( InputIterator f, InputIterator l, const T& val)
	{
		typename iterator_traits<InputIterator>::difference_type c = 0;
		for (; f != l; ++f)
			if (*f == val) ++c;
		return c;
	}

}
