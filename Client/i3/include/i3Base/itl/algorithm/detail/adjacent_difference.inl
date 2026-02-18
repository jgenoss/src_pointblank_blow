

namespace i3
{
	template<class InIt, class OutIt> inline
	OutIt	adjacent_difference(InIt f, InIt l, OutIt dest)
	{
		typedef typename i3::iterator_traits<InIt>::value_type		T;

		T val = *f;
		for (*dest = val; ++f != l; )
		{	
			T tmp = *f;
			*++dest = tmp - val;
			val = tmp;
		}
		return ++dest;
	}

	template<class InIt, class OutIt, class Fn2> inline
	OutIt	adjacent_difference(InIt f, InIt l, OutIt dest, Fn2 fn)
	{
		typedef typename i3::iterator_traits<InIt>::value_type		T;

		T val = *f;
		for (*dest = val; ++f != l; )
		{	
			T tmp = *f;
			*++dest = fn(tmp, val);
			val = tmp;
		}
		return ++dest;
	}


}
