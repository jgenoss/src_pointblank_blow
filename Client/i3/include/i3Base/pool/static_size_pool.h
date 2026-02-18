#pragma once

//
// 동일 사이즈에 대해 유일성을 보장해주는 풀에서는 낱개의 풀보다는 static_pool의 개념이 더 알맞음...
// ID를 부여하기 때문에, 이것을 클래스풀로 만드는 것은 어려움...
//
#include "storage_pool_base.h"
#include "static_adjust_align_for_pool.h"

namespace i3
{
	template<class ID, class T>
	void*		static_size_pool_malloc(identity<ID>, identity<T>);

	template<class ID, class T>
	void		static_size_pool_free(identity<ID>, identity<T>, void* data);
	
	template<class ID, class T> struct reserve_static_size_pool : integral_constant<std::size_t, 32> {};

	template<class ID, class T> struct is_multi_thread_static_size_pool : false_type {};
		

	template<class ID, std::size_t ObjSize, std::size_t Reserve>
	class static_size_pool_st : public shared_ginst< static_size_pool_st<ID, ObjSize, Reserve>, tstorage_pool_st<ObjSize, Reserve> >
	{
	};
	
	template<class ID, std::size_t ObjSize, std::size_t Reserve>
	class static_size_pool_mt : public shared_ginst< static_size_pool_mt<ID, ObjSize, Reserve>, tstorage_pool_mt<ObjSize, Reserve > >
	{
	};
	
	template<class ID, std::size_t ObjSize>
	class static_size_pool_mt_large : public shared_ginst< static_size_pool_mt_large<ID, ObjSize>, tstorage_pool_mt_large<ObjSize> >
	{
	};
	
	namespace detail
	{
		template<class ID, class T>
		struct select_static_size_pool_type 
		{
			enum { 	object_size = static_adjust_size_for_pool<T>::value };
			enum {	object_alignment = static_adjust_align_for_pool<T>::value };
			static const std::size_t reserve = reserve_static_size_pool<ID, T>::value;

			static const bool is_multi_thread = is_multi_thread_static_size_pool<ID, T>::value;
			static const bool is_large_object = ( object_size > maximum_object_size );
			
			typedef typename mpl::if_c< !is_multi_thread,	static_size_pool_st<ID, object_size, reserve>,
					typename mpl::if_c< !is_large_object,	static_size_pool_mt<ID, object_size, reserve>, 
															static_size_pool_mt_large<ID, object_size> >::type >::type	type;

		};
	}

	// 템플릿파라미터 ID는 개별 인스턴스로 만들기위해 추가됨...
	template<class ID> 
	class static_size_pool
	{
	public:
		template<class ID2, class T>
		friend void*		static_size_pool_malloc(identity<ID2>, identity<T>);

		template<class ID2, class T>
		friend void			static_size_pool_free(identity<ID2>, identity<T>, void* data);

	private:
		template<class T>
		static void*	allocate() 
		{  
			typedef typename detail::select_static_size_pool_type<ID, T>::type  pool_type;
			return pool_type::i()->allocate();
		}

		template<class T>
		static void		deallocate(void* data) 
		{  
			typedef typename detail::select_static_size_pool_type<ID, T>::type  pool_type;
			return pool_type::i()->deallocate(data);
		}
	};
	
		
}

#include "static_size_pool.inl"

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)
	
	#define static_size_pool_malloc(id, type_obj)		static_size_pool_malloc_dbg(id, type_obj, __FILE__, __LINE__)
	#define static_size_pool_free(id, type_obj, data)	static_size_pool_free_dbg(id, type_obj, data)
	
#else
		
	#define static_size_pool_malloc_dbg(id, type_obj, f, l)		static_size_pool_malloc(id, type_obj)
	#define static_size_pool_free_dbg(id, type_obj, data)		static_size_pool_free(id, type_obj, data)
	
#endif
}
