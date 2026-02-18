
namespace i3
{
	namespace detail
	{
		template<std::size_t AllocSize, std::size_t SizeIndex, bool l> __forceinline
		void*	local_large_object_cache_dispatch<AllocSize, SizeIndex, l>::allocate()
		{
			return allocate_from_malloc( AllocSize, SizeIndex);
		}

		template<std::size_t AllocSize, std::size_t SizeIndex, bool l> __forceinline
		void	local_large_object_cache_dispatch<AllocSize, SizeIndex, l>::deallocate(void* data)
		{
			large_object_header* header = reinterpret_cast<large_object_header*>( uintptr_t(data) - sizeof(large_object_header) );
			private_free( header->raw_data );
		}
		
		template<std::size_t AllocSize, std::size_t SizeIndex> 
		local_large_object_cache_dispatch<AllocSize, SizeIndex, false>::local_large_object_cache_dispatch()
		{
			
		}

		template<std::size_t AllocSize, std::size_t SizeIndex>
		local_large_object_cache_dispatch<AllocSize, SizeIndex, false>::~local_large_object_cache_dispatch()
		{
			large_object_cache_base::cleanup_stack(m_stack);
		}
	
		template<std::size_t AllocSize, std::size_t SizeIndex> inline
		void*		local_large_object_cache_dispatch<AllocSize, SizeIndex, false>::allocate()
		{
			void* result = nullptr;
			if ( ( result = m_stack.pop() ) != nullptr)
				return result;

			return allocate_from_malloc( AllocSize, SizeIndex );
		}

		template<std::size_t AllocSize, std::size_t SizeIndex> __forceinline
		void		local_large_object_cache_dispatch<AllocSize, SizeIndex, false>::deallocate(void* data )
		{
			m_stack.push(data);	
		}


	}

}