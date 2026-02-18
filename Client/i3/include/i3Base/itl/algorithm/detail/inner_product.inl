
namespace i3
{
	template<class InIt1, class InIt2, class T> inline
	T inner_product(InIt1 f1, InIt1 l1, InIt2 f2, T val)
	{	
		for (; f1 != l1; ++f1, ++f2)
			val = val + *f1 * *f2;
		return val;
	}

	template<class InIt1, class InIt2, class T, class Fn2_1, class Fn2_2> inline
	T inner_product(InIt1 f1, InIt1 l1, InIt2 f2, T val, Fn2_1 fn1, Fn2_2 fn2)
	{
		for (; f1 != l1; ++f1, ++f2)
			val = fn1(val, fn2(*f1, *f2));
		return val;
	}

}
