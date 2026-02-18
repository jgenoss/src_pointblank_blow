

namespace i3
{
	template<class InIt, class OutIt> inline
	OutIt partial_sum(InIt f, InIt l, OutIt dest)
	{
		typedef typename i3::iterator_traits<InIt>::value_type	T;
		T val = *f;
		for (*dest = val; ++f != l; *++dest = val)
			val = val + *f;
		return ++dest;
	}

	template<class InIt, class OutIt, class Fn2> inline
	OutIt partial_sum(InIt f, InIt l, OutIt dest, Fn2 fn)
	{
		typedef typename i3::iterator_traits<InIt>::value_type	T;
		T val = *f;
		for (*dest = val; ++f != l; *++dest = val)
			val = fn(val, *f);
		return ++dest;
	}

}
