#pragma once

#include "../../itl/noncopyable.h"
#include "../../itl/vector_basic.h"

#include "../../memory/heap_allocator.h"

namespace i3
{
	//
	//  쓰레드가 디태치되더라도, tlsData가 유지되도록 처리...
	//  새 쓰레드가 tlsData를 재활용한다면 초기화를 스킵...
	//	tls를 위한 allocator를 만들고, 이것을 여기에 다시 반영할 것!!
	//
	class tls_base;

	//		재활용을 위해서, thread_data는 캐싱되어야 한다..
	namespace detail_tls
	{
		class thread_data;

		typedef void*	(*fn_get_value)(const tls_base* t, const thread_data* , size_t);
		typedef i3::vector<tls_base*, heap_allocator<tls_base*> >		tls_list;			// 전역적 리스트로 도중에 안바뀜..
		typedef i3::vector<void*, heap_allocator<void*> > 				slot_list;			// 쓰레드별로 보관되므로 안전...
		typedef i3::vector<fn_get_value, heap_allocator<fn_get_value> >	fn_value_list;

	

		class thread_data : private i3::noncopyable
		{
		public:
			thread_data( heap_handle heapTls, heap_handle safe_heap, DWORD threadID); 
			void*				tls_malloc(size_t n) const;
			void				tls_free(void* ptr) const;
			void*				tls_aligned_malloc(size_t n, size_t align) const;
			void				tls_aligned_free(void* ptr) const;

			void*				tls_safe_malloc(size_t n) const;
			void				tls_safe_free(void* ptr) const;
			void*				tls_safe_aligned_malloc(size_t n, size_t align) const;
			void				tls_safe_aligned_free(void* ptr) const;			
			
			void*				get_value(const tls_base* t, size_t slot) const { return (*this->fn_list[slot])( t, this, slot); }

			void*				get_raw_value(size_t slot) const { return this->slotlist[slot]; }
			DWORD				get_thread_id() const { return this->hthread; }
						
			template<class T>
			void				reserve_value();
			template<class T>
			void				create_value(size_t slot);

		private:
			fn_value_list		fn_list;
			slot_list			slotlist;				// 이 멤버는 쓰레드에 안전..
			const heap_handle	heapTls;				// Tls힙을 설정한다..
			const heap_handle	safe_heap;
			const DWORD			hthread;
			friend class tls_mgr;
		};


		class tls_mgr
		{
		public:
			tls_mgr();
			~tls_mgr();

			void				OnProcessAttach();
			void				OnProcessDetach();
			void				OnThreadAttach();		// tls dll이 구동되기 전 쓰레드를 얻을수 없으나, 이 dll이 가능한한 먼저 실행된다면 문제는 없을 것이다...
			void				OnThreadDetach();		//쓰레드함수가 종료되기 전에 FreeLibrary가 되거나, 메인스레드가 종료되면, 이것이 호출되지 않음..
			
			thread_data*		get_curr_thread_data() const;
			thread_data*		get_thread_data(DWORD hthread) const;

			DWORD				get_num_thread_data() const { return (DWORD)m_usingSlotList.size(); }
	
			void				PushBackTls(tls_base* tls);	// TLS변수가 생성될때만 쓰여지므로, 안전..
			size_t				GetTlsListSize() const;		// TLS변수가 생성될때만 쓰여지므로, 안전..
			
		private:

			typedef i3::vector<thread_data*, heap_allocator<thread_data*> >		UsingSlotList;		// 안전하지 않음..(DllMain에서만 안전)
			typedef i3::vector<thread_data*, heap_allocator<thread_data*> >		ReservedSlotList;	// 안전하지 않음..(DllMain에서만 안전)

			DWORD				m_dwTlsIdx;
			tls_list			m_TlsBaseList;			// 프로세스 종료시까지 이것은 유지될것임..(중간에 생성되어서는 안됨)..(캐싱될 필요는 없음)
			
			UsingSlotList		m_usingSlotList;		//  쓰레드 리스트.. (아이디로 색인)
			ReservedSlotList	m_reservedSlotList;		//	쓰레드 예약리스트..
			
		};
	
		extern I3_EXPORT_BASE tls_mgr	g_TlsManager;
	}
}



#include "tls_mgr.inl"
