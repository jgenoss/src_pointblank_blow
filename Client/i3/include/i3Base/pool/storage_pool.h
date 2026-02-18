#pragma once

// 동기 :	class_object_pool의 경우, 블록 16K가 항상 잡히기 때문에, 매번 효율적이라고 장담하기 곤란함..
//			그래서, 공용풀과 freelist를 다시 사용해, 메모리 낭비를 약간 줄일수 있는 풀을 추가하기로 했음

//
// 제작상 어려움 :	1) 우선 leak_check여부판별과 초기 reserve사이즈를 잡는 부분을 고민해야한다...
//					2) 효율성을 위해, single과 multi 쓰레드를 구별해야한다...
//					3) i3::vector를 사용하는 것이 그다지 좋지 않기 때문에, 이것 대신에 직접 프리리스트를 처리하는 것으로 대체..
//
// 주의 : 디버그용 매크로보다 먼저 선언되어야 함..



#include "storage_pool_base.h"

namespace i3
{
	template<class T> class storage_pool;

	template<class T> 
	void*	storage_pool_malloc(storage_pool<T>* pool_obj);


	template<class T> 
	void	storage_pool_free(storage_pool<T>* pool_obj, void* data);

	template<class T> 
	void*	storage_pool_malloc_no_dbg(storage_pool<T>* pool_obj);

	template<class T> 
	void	storage_pool_free_no_dbg(storage_pool<T>* pool_obj, void* data);




	template<class T>
	class storage_pool 
	{
	public:
		enum { object_alignment = static_adjust_align_for_pool<T>::value };

		template<class T2> 
		friend	void*	storage_pool_malloc(storage_pool<T2>* pool_obj);
		template<class T2> 
		friend	void	storage_pool_free(storage_pool<T2>* pool_obj, void* data);

	private:
		void*		allocate() { return m_pool.allocate(); }
		void		deallocate(void* data) { m_pool.deallocate(data); }
		
		typedef typename detail::select_storage_pool_type<T>::type	pool_type;

		pool_type	m_pool;

		template<class> friend class static_storage_pool; 
	};
	
}

#include "storage_pool.inl"


namespace i3
{

#if defined(_DEBUG) && defined(I3_POOL_DBG)

	#define storage_pool_malloc(pool_obj)						storage_pool_malloc_dbg(pool_obj, __FILE__, __LINE__)
	#define storage_pool_free(pool_obj, data)					storage_pool_free_dbg(pool_obj, data)

#else
	
	#define storage_pool_malloc_dbg(pool_obj, f, l )			storage_pool_malloc(pool_obj)
	#define storage_pool_free_dbg(pool_obj, data)				storage_pool_free( pool_obj, data)
	
#endif
}
