#pragma once

namespace i3
{
	template<class InputIterator, class T> inline 
	InputIterator find_not(InputIterator First, InputIterator Last, const T& Val) 
	{	
		for (; First != Last; ++First)
			if (*First != Val)
				break;
		return First;
	}	
}

