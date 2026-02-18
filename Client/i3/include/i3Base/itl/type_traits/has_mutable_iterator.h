#pragma once

namespace i3
{
	//
	// 컨테이너가 반복자는 존재하는데, const반복자와 일치하는 경우를 잡으면 된다.
	// 이런 컨테이너는 삽입/삭제는 가능하더라도, 원소의 직접 수정이 불가능하다..
	//
	
	namespace detail
	{
		template<class T>
		struct has_mutable_iterator_impl  
		{
			typedef typename iterator_pointer<typename T::iterator>::type	iterator_pointer;
			typedef typename remove_pointer<iterator_pointer>::type			pointee;

			static const bool value = !is_const<pointee>::value;
			typedef integral_constant<bool, value>				type;
		};
	}

	template<class T>
	struct has_mutable_iterator 
	{
		typedef typename mpl::eval_if< 
					has_iterator<T>,
					detail::has_mutable_iterator_impl<T>,			
					false_type>::type type;

		static const bool value = type::value;
	};
	
	
}