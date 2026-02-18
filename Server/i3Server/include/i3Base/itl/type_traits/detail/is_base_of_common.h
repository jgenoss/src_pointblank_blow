#pragma once


// 실제로 is_base_of는 아래 구현을 사용치 않는다..
// 다른 SFINAE 특질에서 필요하면 활용하기 위해 남겨둠...

namespace i3
{
	namespace detail
	{
		template<class B, class D>
		struct is_base_of_host
		{
			operator B const volatile* () const;
			operator D const volatile* ();
		};
			
		template<class B, class D>
		struct is_base_of_test
		{
			template<class T> 
			static yes_type call(D const volatile*, T);		// 여기서 T는 무엇인지 모름..
			static no_type	call(B const volatile*, int);
		};
		
	}
}