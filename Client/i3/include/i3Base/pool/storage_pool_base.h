#pragma once



#include "cache_pool_impl.h"
#include "large_object_cache_dispatch.h"
#include "static_adjust_align_for_pool.h"


namespace i3
{
	//
	// 16K미만을 지원하도록 처리한다...
	//

	template<class T> struct reserve_storage_pool  {  enum { value = 32 };  };
	template<class T> struct is_multi_thread_storage_pool : false_type {};

	
	template<std::size_t ObjSize, std::size_t Reserve>
	class tstorage_pool_st : public detail::cache_pool_st_impl
	{
	public:
		typedef detail::cache_pool_st_impl	base_type;

		tstorage_pool_st() : base_type( ObjSize, Reserve) {}

		void*		allocate() 
		{
			void* result = base_type::allocate_impl();
			if ( result != 0 ) return result;
			return base_type::allocate_from_static_pool(ObjSize);
		}

		void		deallocate(void* data) { base_type::deallocate_impl(data); }
	};

	template<std::size_t ObjSize, std::size_t Reserve>
	class tstorage_pool_mt : public detail::cache_pool_mt_impl
	{
	public:
		typedef detail::cache_pool_mt_impl		base_type;
	
		tstorage_pool_mt() : base_type( Reserve ) {}
		void*		allocate() 
		{
			void* result;
			if ( ( result = base_type::allocate_impl() ) != 0 ) return result;
			return base_type::allocate_from_static_pool(ObjSize);
		}
		void		deallocate(void* data) { base_type::deallocate_impl(data); }
	};

	// 이 경우 static_object_pool과 동일한 전략 처리..
	
	template<std::size_t ObjSize>
	class tstorage_pool_mt_large : public detail::select_large_object_cache_dispatch_with_object_size<ObjSize>::type 
	{
		
	};
	

	namespace detail
	{
		template<class T>
		struct select_storage_pool_type 
		{
			enum { 	object_size = static_adjust_size_for_pool<T>::value };
			enum {	object_alignment = i3::static_adjust_align_for_pool<T>::value };
			static const std::size_t reserve = i3::reserve_storage_pool<T>::value;

			static const bool is_multi_thread = is_multi_thread_storage_pool<T>::value;
			static const bool is_large_object = ( object_size > maximum_object_size );
			
			typedef typename mpl::if_c< !is_multi_thread,	tstorage_pool_st<object_size, reserve>,
					typename mpl::if_c< !is_large_object,	tstorage_pool_mt< object_size, reserve>, 
															tstorage_pool_mt_large<object_size> >::type >::type	type;
		};
	}
}


