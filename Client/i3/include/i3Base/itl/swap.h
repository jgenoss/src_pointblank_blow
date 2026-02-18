#pragma once

#include <cstddef>

namespace i3
{
	
	namespace detail
	{
		template<class T, bool is_empty_type>
		struct swap_helper_by_empty {	__forceinline static void swap_impl(T&, T&) {} };
		
		template<class T>
		struct swap_helper_by_empty<T, false> 
		{  
			__forceinline static void swap_impl(T& left, T& right) {	using std::swap;	swap(left, right);	}
		};

		template<class T> __forceinline
		void swap_impl(T& left, T& right)
		{
			swap_helper_by_empty<T, is_empty<T>::value >::swap_impl(left, right);	//using std::swap;	swap(left, right);		
		}
		
		template<class T, std::size_t N> __forceinline
		void swap_impl(T (& left)[N], T (& right)[N])
		{
			for (std::size_t i = 0; i < N; ++i)
			{
				detail::swap_impl(left[i], right[i]);
			}
		}
		


	}

	template<class T1, class T2> __forceinline
	void swap(T1& left, T2& right)
	{
		detail::swap_impl(left, right);
	}
	


}