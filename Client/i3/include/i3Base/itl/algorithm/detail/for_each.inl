
namespace i3
{
	template<class InputIterator, class Fn1> inline
	Fn1 for_each(InputIterator First, InputIterator Last, Fn1 Func)
	{	
		for (; First != Last; ++First)
			Func(*First);
		return Func;
	}
}
