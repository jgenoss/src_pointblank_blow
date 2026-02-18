
namespace i3
{

	template<class InputIt, class OutputIt, class T> inline
	OutputIt remove_copy(InputIt f, InputIt l, OutputIt dest, const T& val)
	{
		for (; f != l; ++f)
			if (!(*f == val)) *dest++ = *f;
		return dest;
	}

}
