#pragma once

#include "../../itl/noncopyable.h"
#include "../../itl/vector_basic.h"

#include "../../memory/heap_allocator.h"
#include "../../pool/cs.h"

namespace i3
{
	//
	namespace detail_tls
	{
		typedef i3::vector<void*, heap_allocator<void*> > 				slot_list;			// 쓰레드별로 보관되므로 안전...
	
		struct thread_data : private i3::noncopyable
		{
			thread_data( heap_handle heapTls, heap_handle safe_heap, DWORD threadID); 
		
			slot_list			slotlist;				
			const heap_handle	heapTls;				// Tls힙을 설정한다..
			const heap_handle	safe_heap;
			const DWORD			hthread;
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

			DWORD				get_num_thread_data() const { return (DWORD)m_thread_data_list.size(); }
	
			unsigned long		get_num_tls_ptr() const;		// TLS변수가 생성될때만 쓰여지므로, 안전..
			I3_EXPORT_BASE unsigned long	obtain_tls_idx();				
			I3_EXPORT_BASE void				return_tls_idx(unsigned long slot);
		private:

			typedef i3::vector<thread_data*, heap_allocator<thread_data*> >		thread_data_list;		// 안전하지 않음..(DllMain에서만 안전)
			typedef i3::vector<thread_data*, heap_allocator<thread_data*> >		reserved_td_list;	// 안전하지 않음..(DllMain에서만 안전)
			typedef i3::vector<unsigned long, heap_allocator<unsigned long> >	reserved_slot_list;

			DWORD				m_dwTlsIdx;
			unsigned long		m_num_tls;		// 무조건 증가하며 줄지 않음..(전역/정적 변수를 가정해야한다..)
		
			thread_data_list	m_thread_data_list;		//  쓰레드 리스트.. (아이디로 색인)
			reserved_td_list	m_reserved_td_list;		//	쓰레드 예약리스트..
			
			reserved_slot_list	m_reserved_slot_list;	// 슬롯 인덱스 배정에 쓴다..
			i3::cs				m_slot_list_cs;			// 크리티컬섹션(변수 슬롯인덱스 지정과 반납에만 쓴다.)
		};
	
		extern I3_EXPORT_BASE tls_mgr	g_TlsManager;
	}
}



#include "tls_mgr.inl"
