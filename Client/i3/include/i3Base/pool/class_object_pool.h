#pragma once

#include "static_object_pool.h"
#include "static_pool.h"
#include "../itl/mpl/mpl_identity.h"

#pragma push_macro("new")

#undef new


namespace i3
{
	
//
//	다음의 풀은 mine 오브젝트 타입에 의해서 풀이 달라지므로, 다음번 파생클래스의 상속에는 반드시 new/delete를 재정의하거나, class_object_pool을 
//  계속 덧붙여주어야함..
//  오브젝트 단위별로 적용되므로, 배열스타일 연산자는 사용하지 않는데..사용하게되면 common_pool과 동일하게 처리함..

	template <class mine, class base = i3::null_type>
	class class_object_pool : public base
	{
	public:
		class_object_pool(){}
		template<class T>
		class_object_pool(T arg) : base(arg) {}			// 인자생성자를 위해 계속추가될 가능성있음..
		template<class T1, class T2>
		class_object_pool(T1 a1, T2 a2) : base(a1, a2) {}
		template<class T1, class T2, class T3>
		class_object_pool(T1 a1, T2 a2, T3 a3) : base(a1, a2, a3) {}
		template<class T1, class T2, class T3, class T4>
		class_object_pool(T1 a1, T2 a2, T3 a3, T4 a4) : base(a1, a2, a3, a4) {}
		template<class T1, class T2, class T3, class T4, class T5>
		class_object_pool(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) : base(a1, a2, a3, a4, a5) {}

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

#include "class_object_pool.inl"

#pragma pop_macro("new")
