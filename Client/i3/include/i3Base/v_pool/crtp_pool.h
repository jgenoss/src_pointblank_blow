#pragma once

//
// 현재 i3Engine의 객체풀은 컴파일시점이 아닌 실행 초기화시점에서 지정되는 경향이 조금 있기 때문에 가상함수동작이 포함된 클래스가 요구됨..
//
#include "v_pool_info.h"

#define crtp_pool_arity_limit 5

namespace i3
{
	template<class T, class B = i3::null_type>
	class crtp_pool : public B 
	{
	public:
		crtp_pool() {}
#define repeat_crtp_pool_cons(z, n, d) \
		template<pp_enum_params_1(n, class A)> \
		crtp_pool( pp_enum_binary_params_1(n, A, a) ) : B(pp_enum_params_1(n, a)) {}
		pp_repeat_from_to(1, pp_inc(crtp_pool_arity_limit), repeat_crtp_pool_cons, ~)
#undef	repeat_crtp_pool_cons

		static void		create_explicit() { _create_pool_class_info(); }

		template<class Lambda>
		static void		set_pool() 
		{
			typedef typename mpl::apply1<Lambda, T>::type		pool_type;
			ms_pool_info->set_pool( create_v_pool_ptr<pool_type>() );
		}

// new/delete는 오버라이딩 처리하면 되지만...
// 일반생성/소거함수를 둔다면 file/line문제로 함수 이름이 너무 단순하면 안된다..
#if !defined(_DEBUG)		// 릴리즈..
		static void*	operator	new (std::size_t size) { return ms_pool_info->allocate();  }
#else						// 디버그..
		static void*	operator	new (std::size_t size) {  return ms_pool_info->allocate(__FILE__, __LINE__);    }	// 이렇게 들어오면 정보를 잃는것임..
		static void* 	operator	new(std::size_t size, const char* file, int line) { return ms_pool_info->allocate(size, file, line); }
		static void		operator	delete(void *ptr, const char* file, int line)  {   ms_pool_info->deallocate(ptr); }
#end		
		static void		operator	delete(void *ptr)  {   ms_pool_info->deallocate(ptr);   }		

	private:
		v_pool_info*			_create_pool_class_info() 
		{	
			return i3::create_global_with_atexit<v_pool_info, ms_pool_info>(i3::identity<T>()); 
		}

		static v_pool_info*		ms_pool_info;			// 
	};

	template<class T, class B>
	v_pool_info*			crtp_pool<T, B>::ms_pool_info = crtp_pool<T, B>::_create_pool_class_info();

}
