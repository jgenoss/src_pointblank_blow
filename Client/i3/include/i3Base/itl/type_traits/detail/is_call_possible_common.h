#pragma once

//#pragma warning(disable : 4913)		// 이 경고를 제거한다..나중에 웹에 기록해두어야함.(2012.12.06.수빈)

namespace i3
{

	// 아래 클래스들을 유심이 보면, 임의 함수객체나 함수포인터의 리턴타입을 꺼내올수 있을 것임...

	namespace detail
	{
		class void_exp_result {};		// void로 평가되면, void&같은것은 불가능하므로, 아래 연산자 함수를 거치지 않으므로, 
										// 디폴트 콤마표현식으로 넘어가는 객체임..

//		template <class R> const R&	operator,(const R&, void_exp_result);	// 무언가 리턴되면 void_exp_result를 무시하게 만듬..
//		template <class R> R&		operator,(R&, void_exp_result);			// 오버로딩함수목록에서 검색이 안되면, 콤마디폴트동작으로 넘어감..
		
		template <class R> 
		struct return_value_check 
		{ 
			template <class R> friend const R&	operator,(const R&, void_exp_result);	// 무언가 리턴되면 void_exp_result를 무시하게 만듬..
			template <class R> friend R&		operator,(R&, void_exp_result);			// 오버로딩함수목록에서 검색이 안되면, 콤마디폴트동작으로 넘어감..

			static yes_type test(R); 
			static no_type	test(...); 
			static no_type	test(no_type);					// 다음 2개 오버로드함수는 없어도 되지만,원본대로 남김..
			static no_type	test(void_exp_result); 
		}; 

		template <> 
		struct return_value_check<void> 
		{ 
			static yes_type	test(...); 
			static no_type	test(no_type); 
		}; 


	}
}