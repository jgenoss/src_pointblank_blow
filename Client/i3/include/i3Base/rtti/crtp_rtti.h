#pragma once

//
// 신규 RTTI의 개요 : CRTP를 쓴다.  
// 주된 단점 : 디버깅 워치창에서 베이스클래스를 곧바로 확인하지 못하고, +단추를 열어야 한다는 점이 있다..
// 좀 나아진 점 : 클래스 내부에 선언해야하는 매크로는 모두 제거되고, 매크로 대신에 CRTP에 베이스클래스를 끼운다..
// 좀 나아진 점2 : RegisterMeta와 같은 초기화함수 호출이 필요가 없다.
// 특징적인 점 : 일반적인 RTTI구현과 달리 계층 배열을 두기때문에, 타입의 상속계층검사는 2회 비교로 끝낼수 있다..
// 특징적인 점2 : RTTI클래스를 제외한 모든 접근 사용루틴은 일반함수이다..
// RTTI는 수집정보나 팩토리개념은 원래 없기 때문에, 이부분은 따로 클래스를 두고 만든다...

#include "rtti_info.h"
#include "detail/crtp_rtti_impl.h"

#define rtti_arity_limit 5

#define crtp_rtti_global_init(type) static i3::crtp_rtti_initializer<type>	pp_cat(pp_cat(_crtp_rtti_initializer_, type), _; )	

namespace i3
{
	// crtp클래스...
	template<class T, class B = i3::null_type>
	class crtp_rtti : public B 
	{
	public:
		crtp_rtti() {}
#define repeat_rtti_cons(z, n, d) \
	template<pp_enum_params_1(n, class A)> \
		crtp_rtti( pp_enum_binary_params_1(n, A, a) ) : B(pp_enum_params_1(n, a)) {}
		pp_repeat_from_to(1, pp_inc(rtti_arity_limit), repeat_rtti_cons, ~)
#undef	repeat_rtti_cons

		static void	create_explicit() { _create_rtti_class_info(); }
		static const rtti_info*	static_rtti()	{ return ms_rtti_info; }
		virtual rtti_info*		get_rtti()		{ return ms_rtti_info; }
		virtual bool same_of( const rtti_info* static_info) const { return ms_rtti_info->same_of(static_info); }
		virtual bool kind_of( const rtti_info* static_info) const { return ms_rtti_info->kind_of(static_info); }

	private:
		rtti_info*			_create_rtti_class_info()
		{
			return i3::create_global_with_atexit<rtti_info, ms_rtti_info>(i3::identity<B>());
		}
		static rtti_info*		ms_rtti_info;			// 
	};

	template<class T, class B>
	rtti_info*			crtp_rtti<T, B>::ms_rtti_info = crtp_rtti<T, B>::_create_rtti_class_info();
		


}
