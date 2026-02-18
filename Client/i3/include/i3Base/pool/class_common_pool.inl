
#if !defined(pp_is_iterating)

#include "../itl/preprocessor/pp_iterate.h"

namespace i3
{

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new (std::size_t size) 
	{		
		return static_pool_type_malloc( identity<m>(), size); 
	}

	template <class m, class b> __forceinline
	void class_common_pool<m,b>::operator delete (void *ptr) 
	{	
		static_pool_type_free(  identity<m>(), ptr);
	}

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new[] (std::size_t size)	
	{	
		return static_pool_type_malloc(identity<m>(), size); 
	}

	template <class m, class b> __forceinline
	void class_common_pool<m,b>::operator delete[] (void *ptr)	
	{	
		static_pool_type_free(identity<m>(), ptr);
	}

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new ( std::size_t size, void* place ) {	 return ::operator new( size, place );	}		// placement new는 큰 의미는 없음..

	template <class m, class b> __forceinline
	void	 class_common_pool<m,b>::operator delete ( void * ptr, void * place ) {	 ::operator delete ( ptr, place ); }

	template <class m, class b> __forceinline
	void *	class_common_pool<m,b>::operator new [] ( std::size_t size, void * place ) {  return ::operator new( size, place ); }

	template <class m, class b> __forceinline
	void	class_common_pool<m,b>::operator delete [] ( void * ptr, void * place ) { ::operator delete ( ptr, place ); }

#ifdef _DEBUG
	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new (std::size_t size,int ,const char *file,int line) 
	{	
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new[] (std::size_t size,int ,const char* file,int line) 
	{	
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new (std::size_t size,const char *file,int line) 
	{	
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}

	template <class m, class b> __forceinline
	void* class_common_pool<m,b>::operator new[] (std::size_t size,const char* file,int line) 
	{	
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}



	template <class m, class b> __forceinline
	void  class_common_pool<m,b>::operator delete (void *ptr,int ,const char* ,int )	
	{	
		static_pool_type_free_dbg(identity<m>(), ptr);
	}

	template <class m, class b> __forceinline
	void  class_common_pool<m,b>::operator delete[] (void *ptr,int ,const char* ,int ) 
	{ 	
		static_pool_type_free_dbg(identity<m>(), ptr);
	}
#endif	


	
}

#define		pp_iteration_params_1	(3, (2, class_common_pool_arity_limit, "class_common_pool.inl"))				// 1단계 파일반복..
#include	pp_iterate()

#else		// pp_is_iterating

#define i_				pp_frame_iteration(1)
	
namespace i3 
{
	template <class m, class b>
	template< pp_enum_params_1(i_, class T)> 
	class_common_pool<m, b>::class_common_pool( pp_enum_binary_params_1(i_, T, a) ) : b( pp_enum_params_1(i_, a) )
	{
	}
		
}
	
#undef i_

#endif