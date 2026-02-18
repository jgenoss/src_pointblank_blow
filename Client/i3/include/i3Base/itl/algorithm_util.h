#pragma once 

#include "pair.h"


namespace i3
{
	namespace algorithm_util 
	{
		template<class ForwardIterator, class T, class Pred> inline
		i3::pair<ForwardIterator, bool> binary_find(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr )
		{
			ForwardIterator it = i3::lower_bound(First, Last, Val, Pr);	
			bool result =  ( it != Last && !Pr(Val, *it) );
			return i3::pair<ForwardIterator, bool>(it, result);
		}

		template<class ForwardIterator, class T> inline
		i3::pair<ForwardIterator, bool> binary_find(ForwardIterator First, ForwardIterator Last, const T& Val )
		{
			ForwardIterator it = i3::lower_bound(First, Last, Val);	
			bool result =  ( it != Last && !(Val < *it) );			// < operator에만 의존해야함..
			return i3::pair<ForwardIterator, bool>(it, result);
		}
		

		template<class InputIterator, class Fn1> inline
		Fn1 for_each_2nd(InputIterator First, InputIterator Last, Fn1 Func)
		{	
			for (; First != Last; ++First)
				Func(First->second);
			return Func;
		}
		
		template<class InputIterator, class T> inline
		InputIterator find_2nd(InputIterator First, InputIterator Last, const T& Val)
		{
			for (; First != Last; ++First)
				if (First->second == Val)
					break;
			return First;
		}

		template<class InputIterator, class Pred> inline
		InputIterator find_if_2nd(InputIterator First, InputIterator Last, Pred pr)
		{
			for (; First != Last; ++First) 
				if (pr(First->second)) break;
			return First;
		}


	}
	
	namespace au = algorithm_util;
}