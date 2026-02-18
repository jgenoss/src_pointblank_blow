
namespace i3
{
	template<class InIt, class OutIt, class T> inline
	OutIt replace_copy(InIt f, InIt l, OutIt dest, const T& old_val, const T& new_val)
	{
		for (; f != l; ++f, ++dest)
			*dest = (*f == old_val) ? new_val : *f;
		return dest;
	}
}
