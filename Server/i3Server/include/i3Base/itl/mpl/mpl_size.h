#pragma once

#include "mpl_size_fwd.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_begin_end.h"
#include "mpl_distance.h"


namespace i3
{
	//
	// 메타함수의 경우, 외부인터페이스는 곧바로 결과를 내게하고, 세부구현은 apply쪽을 명기하여 처리하게끔 한다..
	// 현재까지는 외부메타함수의 내부구현은 언제나 impl접미사와 함께 template apply를 적용한다..
	// 메타함수에서 제일 중요한것은 구현이라기보다는 차라리 인터페이스이다...인터페이스의 통일이 제일 중요하다..
	// 강조) 지금부터 처리해야하겠지만, 템플릿 타입이름은 이름의 아이디어 제한으로 언제나 대문자로 시작하게 한다..
	
	namespace mpl
	{
		// 아래는 일반구현으로 당분간은 쓰이지 않을 것으로 예상된다..
		template<class Tag>
		struct size_impl
		{
			template<class Seq> struct apply : distance<typename begin<Seq>::type, typename end<Seq>::type> {};
		};
		
		// traits lambda spec
		template<> struct size_impl<non_sequence_tag> {};
		

			
		template< class Seq = na >
		struct size : size_impl<typename sequence_tag<Seq>::type >:: template apply<Seq>::type
		{
		};

	}
}


#define na_spec (1, size)
#include "aux_/mpl_na_spec.h"

