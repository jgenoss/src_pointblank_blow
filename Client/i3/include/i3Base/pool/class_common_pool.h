#pragma once

#include "static_pool.h"

#pragma push_macro("new")

#undef new

#define class_common_pool_arity_limit		5

#include "../itl/preprocessor/pp_repeat_from_to.h"
#include "../itl/preprocessor/ext/pp_enum_params_1.h"
#include "../itl/preprocessor/ext/pp_enum_binary_params_1.h"


namespace i3
{
//
//  class_common_pool의 경우는...적당한 부모 클래스에 한번만 적용해도 무방함...한번 적용하면 모든 하위클래스에 영향을 줌..
//  그러나, 사용하는 실제클래스의 옆에 일일이 붙이는 것을 권장함.... (이유 : 정렬바이트를 정확히 맞추기 위함)
//

	template <class mine, class base = i3::null_type>
	class class_common_pool : public base
	{
	public:
		class_common_pool(){}
		template<class T>
		class_common_pool(T arg) : base(arg) {}			// 인자생성자를 위해 계속추가될 가능성있음..

#define repeat_class_common_pool_cons(z, n, d) \
		template<pp_enum_params_1(n, class T)> \
		class_common_pool( pp_enum_binary_params_1(n, T, a) );
		pp_repeat_from_to(2, pp_inc(class_common_pool_arity_limit), repeat_class_common_pool_cons, ~)
#undef	repeat_class_common_pool_cons


		static void* operator	new (std::size_t size); 
		static void* operator	new[] (std::size_t size);
		static void* operator	new ( std::size_t size, void* place );
		static void * operator	new[] ( std::size_t size, void * place );

		static void operator	delete (void *ptr);
		static void operator	delete[] (void *ptr);
		static void	 operator	delete ( void * ptr, void * place );		
       	static void operator	delete[] ( void * ptr, void * place );

#ifdef _DEBUG
		static void* operator new (std::size_t size,int iBlockState,const char *file,int line);
		static void* operator new[] (std::size_t size,int iBlockState,const char* file,int line);
		
		static void* operator new (std::size_t size,const char *file,int line);
		static void* operator new[] (std::size_t size,const char* file,int line);

		static void operator delete (void *ptr,int iBlockState,const char* file,int line);
		static void operator delete[] (void *ptr,int iBlockState,const char* file,int line);
#endif		
	};



	
}

#include "class_common_pool.inl"

#pragma pop_macro("new")
