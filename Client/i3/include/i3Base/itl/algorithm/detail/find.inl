
#include "memchr_trait.h"

namespace i3
{
	namespace detail_find
	{
		// TEMPLATE FUNCTION find
		template<class InputIterator, class T> inline 	
		typename i3::disable_if<detail::is_memchr_wmemchr_enable_it_v<InputIterator, T>,
		InputIterator>::type Find(InputIterator First, InputIterator Last, const T& Val)
		{	// find first matching val
			for (; First != Last; ++First)
				if (*First == Val)
					break;
			return First;
		}
		
		template<class InputIterator, class T> inline 
		typename i3::enable_if< detail::is_memchr_enable_it_v<InputIterator, T>,
		InputIterator>::type Find(InputIterator First, InputIterator Last, const T& Val)
		{	// find first char that matches val
			First = (InputIterator)::memchr(First, int(Val), Last - First);
			return (First == 0 ? InputIterator(Last) : InputIterator(First) );
		}

		template<class InputIterator, class T> inline 
		typename i3::enable_if< detail::is_wmemchr_enable_it_v<InputIterator, T>,
		InputIterator>::type Find(InputIterator First, InputIterator Last, const T& Val)
		{
			First = (InputIterator)::wmemchr((const wchar_t*)First, wchar_t(Val), Last - First);
			return (First == 0 ? InputIterator(Last) : InputIterator(First) );
			
		}

	}

	template<class InputIterator, class T> inline 
	InputIterator find(InputIterator First, InputIterator Last, const T& Val) {	return  detail_find::Find(First, Last, Val); }	

}
