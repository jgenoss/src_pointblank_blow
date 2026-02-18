#pragma once

#include "static_storage_pool.h"
#include "static_pool.h"			// 이미 포함되었지만, 한번더 확인삼아 포함..

#pragma push_macro("new")

#undef new


namespace i3
{
	
	template <class mine, class base = i3::null_type>
	class class_storage_pool : public base
	{
	public:
		class_storage_pool(){}
		template<class T>
		class_storage_pool(T arg) : base(arg) {}			// 인자생성자를 위해 계속추가될 가능성있음..
		template<class T1, class T2>
		class_storage_pool(T1 a1, T2 a2) : base(a1, a2) {}
		template<class T1, class T2, class T3>
		class_storage_pool(T1 a1, T2 a2, T3 a3) : base(a1, a2, a3) {}
		template<class T1, class T2, class T3, class T4>
		class_storage_pool(T1 a1, T2 a2, T3 a3, T4 a4) : base(a1, a2, a3, a4) {}
		template<class T1, class T2, class T3, class T4, class T5>
		class_storage_pool(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) : base(a1, a2, a3, a4, a5) {}

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

#include "class_storage_pool.inl"

#pragma pop_macro("new")
