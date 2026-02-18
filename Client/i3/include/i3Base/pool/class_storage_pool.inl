
namespace i3
{

	////////////////////////////////////////////////////////////////

	template <class m, class b> __forceinline
	void* class_storage_pool<m,b>::operator new (std::size_t ) 
	{	
		return static_storage_pool_malloc( identity<m>() );
	}


	template <class m, class b> __forceinline
		void class_storage_pool<m,b>::operator delete (void *ptr) 
	{	
		static_storage_pool_free( identity<m>(), ptr );
	}

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new[] (std::size_t size)
	{
		return static_pool_type_malloc(identity<m>(), size); 
	}

	template <class m, class b> __forceinline
		void class_storage_pool<m,b>::operator	delete[] (void *ptr)
	{
		static_pool_type_free(identity<m>(), ptr);
	}

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new ( std::size_t size, void* place ) {	 return ::operator new( size, place );	}		// placement new는 큰 의미는 없음..

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator	new[] ( std::size_t size, void * place ) { return ::operator new( size, place );	}	


	template <class m, class b> __forceinline
		void	 class_storage_pool<m,b>::operator delete ( void * ptr, void * place ) {	 ::operator delete ( ptr, place ); }

	template <class m, class b> __forceinline
		void	 class_storage_pool<m,b>::operator	delete[] ( void * ptr, void * place ){ ::operator delete ( ptr, place ); }


#ifdef _DEBUG
	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new (std::size_t ,int ,const char *file,int line) 
	{	
		return static_storage_pool_malloc_dbg( identity<m>(), file, line );
	}

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new[] (std::size_t size,int ,const char* file,int line)
	{
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new (std::size_t ,const char *file,int line) 
	{	
		return static_storage_pool_malloc_dbg( identity<m>(), file, line );
	}

	template <class m, class b> __forceinline
		void* class_storage_pool<m,b>::operator new[] (std::size_t size,const char* file,int line)
	{
		return static_pool_type_malloc_dbg(identity<m>(), size, file, line);
	}

	template <class m, class b> __forceinline
		void  class_storage_pool<m,b>::operator delete (void *ptr,int ,const char* ,int )	
	{	
		static_storage_pool_free_dbg( identity<m>(), ptr);
	}

	template <class m, class b> __forceinline
		void class_storage_pool<m,b>::operator delete[] (void *ptr,int iBlockState,const char* ,int )
	{
		static_pool_type_free_dbg(identity<m>(), ptr);
	}

#endif	

}
