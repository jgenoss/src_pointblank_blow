
namespace i3
{

	template<class InputIt, class OutputIt, class Pred> inline
	OutputIt remove_copy_if(InputIt f, InputIt l, OutputIt dest, Pred pr)
	{
		for (; f != l; ++f)
			if (!pr(*f)) *dest++ = *f;
		return dest;
	}

}
