#pragma once

#include "mem_block_header.h"
#include "../tls/tls_ptr.h"
#include "align_up_down.h"

namespace i3
{
	namespace detail
	{
		class I3_EXPORT_BASE cache_pool_st_impl
		{
		protected:
			cache_pool_st_impl(std::size_t size, std::size_t capacity);
			cache_pool_st_impl();
			~cache_pool_st_impl(); 

			void*					allocate_impl();
			void					deallocate_impl(void* data);
			void*					allocate_from_static_pool( std::size_t size);
		private:
			
			void					init_freelist(std::size_t size);

			free_object*			m_freelist;
			std::size_t				m_capacity;
		};

		class I3_EXPORT_BASE cache_pool_mt_impl
		{
		protected:
			cache_pool_mt_impl(std::size_t capacity) : m_init_capacity(capacity) {}
			cache_pool_mt_impl() : m_init_capacity(0) {}
			void					set_capacity(std::size_t capacity) { m_init_capacity = capacity; }
		
			void*					allocate_impl();
			void					deallocate_impl(void* data);
			void*					allocate_from_static_pool( std::size_t size);
			
		private:
			
			__declspec( align(64) )	
			struct tls_data // 캐시경계를 위해 128바이트로 불림..
			{
				tls_data() : freelist(0), capacity(0), freelist_tss(0) {}
				~tls_data();
				void				clean_up_freelist();
				free_object*		freelist;
				size_t				capacity;

				size_t				padding_front[ (pool_block_header_alignment - sizeof(free_object*) - sizeof(size_t) ) / sizeof(size_t)];	
				free_object*		freelist_tss;
				size_t				padding_back[ (pool_block_header_alignment - sizeof(free_object*)) / sizeof(size_t)];						// 캐시라인분리
			};

			static void*			allocate_from_freelist(tls_data* td);
			static void*			allocate_from_freelist_tss(tls_data* td);
			static void				deallocate_to_freelist(tls_data* td,  void* data);
			static void				deallocate_to_freelist_tss(tls_data* td,  void* data);

		
			i3::tls_ptr< tls_data >		m_tls_pool;
			std::size_t				m_init_capacity;
		};
	

		
	}
}

#include "cache_pool_impl.inl"