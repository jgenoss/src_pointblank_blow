

namespace i3
{
	template<class InIt, class T> inline
	T accumulate(InIt f, InIt l, T val)
	{
		for (; f != l; ++f) val = val + *f;
		return val;
	}

	template<class InIt, class T, class Fn2> inline
	T accumulate(InIt f, InIt l, T val, Fn2 fn)
	{
		for (; f != l; ++f) val = fn(val, *f);
		return val;
	}

}