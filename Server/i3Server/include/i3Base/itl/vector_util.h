#pragma once

#include "vector.h"
// 이게 선언.. 나중에 특수화를 추가할 것임..지금은 이정도로 마무리..
namespace i3
{
	namespace vector_util
	{
		template<class T, class A, class U>
		void	set_value_force(i3::vector<T, A>& inoutCtn, size_t idx, const U& value);
			
		template<class T, class A, class U, class V> 
		void	set_value_force(i3::vector<T, A>& inoutCtn, size_t idx, const U& value, const V& null_value);


		template<class T, class A>
		bool	is_value_valid(const i3::vector<T, A>& Ctn, size_t idx );

		template<class T, class A, class U>
		bool	is_value_valid(const i3::vector<T, A>& Ctn, size_t idx, const U& nullValue );
		

		template<class T, class A>
		void	remove_duplicates(i3::vector<T, A>& inout_ctn);
		
		template<class T, class A, class U>
		typename i3::vector<T, A>::difference_type	index_of( const i3::vector<T, A>& ctn, const U& obj);


	}

	namespace vu = vector_util;

}

#include "detail/vector_util.inl"
