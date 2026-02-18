#pragma once 

#include "algorithm/lexicographical_compare.h"
#include "functional.h"
#include "../string/i3string.h"

namespace i3
{
	namespace functor_util
	{
		
		struct delete_fun
		{
			template<class T> inline void operator()(const T* ptr) const
			{
				NetworkDump_Free( const_cast<T*>(ptr), __FILE__, __LINE__);
				delete ptr;
			}
		};

		struct safe_delete_fun
		{
			template<class T> void	operator()(const T*& p) const
			{
				NetworkDump_Free( const_cast<T*>(p), __FILE__, __LINE__);
				delete p; p = nullptr;
			}
		};

	
		template<class Fun>
		struct functor_safe_t
		{
			explicit functor_safe_t(Fun f) : f(f) {}

			template<class T> inline void operator()(T* ptr) const 
			{
				if (ptr != nullptr) f(ptr);
			}
			Fun f;
		};
		
		template<class Fun> inline 
		functor_safe_t<Fun> functor_safe(Fun f) 
		{
			return functor_safe_t<Fun>(f);
		}

		template<class R, class T> inline
		functor_safe_t< mem_fun_t<R, T> > mem_fun_safe(R (T::*Pm)()) {	  return	functor_safe_t<	i3::mem_fun_t<R, T>	>(i3::mem_fun_t<R, T>(Pm)); }

		template<class R, class T, class Arg> inline
		functor_safe_t< mem_fun1_t<R, T, Arg> > mem_fun_safe(R (T::*Pm)(Arg)) {	return functor_safe_t<	i3::mem_fun1_t<R, T, Arg> >(i3::mem_fun1_t<R, T, Arg>(Pm));	}

		template<class R, class T> inline
		functor_safe_t< const_mem_fun_t<R, T> > mem_fun_safe(R (T::*Pm)() const) {	 return functor_safe_t<	i3::const_mem_fun_t<R, T> >(i3::const_mem_fun_t<R, T>(Pm)); }

		template<class R, class T, class Arg> inline
		functor_safe_t< const_mem_fun1_t<R, T, Arg> > mem_fun_safe(R (T::*Pm)(Arg) const) {	return functor_safe_t<i3::const_mem_fun1_t<R, T, Arg> >(i3::const_mem_fun1_t<R, T, Arg>(Pm));	}

		struct less_char_nocase 
		{
			template<class Ch>
			bool operator()( Ch c1, Ch c2 ) const { 	return toupper(c1) < toupper(c2);	}
		};

		struct less_string_nocase
		{	
			template<class Ch, class Trait, class Alloc>
			bool operator()(const i3::basic_string<Ch, Trait, Alloc>& str1, const i3::basic_string<Ch, Trait, Alloc>& str2 ) const
			{
				return lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), less_char_nocase() );
			}
		};

		struct equal_char_nocase
		{
			template<class Ch> 	bool operator()( Ch c1, Ch c2 ) const { 	return toupper(c1) == toupper(c2);	}
		};
	}

	namespace fu = functor_util;
}
