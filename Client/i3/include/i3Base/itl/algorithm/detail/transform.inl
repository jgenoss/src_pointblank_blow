

namespace i3
{

#pragma warning(push)
#pragma warning(disable : 4244)				// 알고리즘 함수객체의 경우, 형변환 경고를 막음..(MS기준)

	template<class InputIterator, class OutputIterator, class Fn1> inline
	OutputIterator transform(InputIterator First, InputIterator Last, OutputIterator Dest, Fn1 Func)
	{
		for (; First != Last; ++First, ++Dest)
			*Dest = Func(*First);
		return (Dest);
	}

	template<class InputIterator1, class InputIterator2, class OutputIterator, class Fn2> inline
	OutputIterator transform(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, OutputIterator Dest, Fn2 Func)
	{
		for (; First1 != Last1; ++First1, ++First2, ++Dest)
			*Dest = Func(*First1, *First2);
		return (Dest);
	}


#pragma warning(pop)

}