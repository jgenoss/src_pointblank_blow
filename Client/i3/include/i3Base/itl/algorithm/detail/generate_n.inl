
namespace i3
{
	template<class OutIt, class Diff, class Fn0> inline
	void	generate_n(OutIt dest, Diff count, Fn0 fn)
	{
		for (; 0 < count; --count, ++dest)
			*dest = fn();
	}
}
