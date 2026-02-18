#pragma once

namespace i3
{
	template<class InputIterator, class Pred> inline 
	InputIterator find_not_if(InputIterator First, InputIterator Last, Pred pr) 
	{	
		for (; First != Last; ++First)
			if (!pr(*First) )
				break;
		return First;
	}	
}

