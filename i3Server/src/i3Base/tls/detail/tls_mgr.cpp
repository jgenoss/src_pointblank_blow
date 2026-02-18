#include "i3CommonType.h"

#include <iostream>
#include <functional>

#include "tls/detail/tls_mgr.h"
#include "tls/detail/tls_base.h"
//


namespace i3
{
	namespace detail_tls 
	{
	
	tls_mgr::tls_mgr() : m_dwTlsIdx(0), m_TlsBaseList( heap_allocator<tls_base*>( i3::get_process_heap() ) ), 
		m_usingSlotList( heap_allocator< thread_data*>( i3::get_process_heap() ) ),
		m_reservedSlotList(heap_allocator< thread_data*>( i3::get_process_heap() ) )
	{
		
	}

	tls_mgr::~tls_mgr() 
	{
		// 슬롯 내의 모든 tls데이터를 응용프로그램 종료시에 삭제한다..(그전까지는 삭제가 없었음)
		// 슬롯리스트들에 대해서 모두 Cleanup처리..

		const size_t num_slotlist = m_usingSlotList.size();
		for ( size_t i = 0 ; i < num_slotlist ; ++i )
		{
			thread_data* data = m_usingSlotList[i];
			heap_handle h = data->heapTls;
			heap_handle sh = data->safe_heap;
			data->~thread_data();
			i3::heap_free(h, data);
			i3::heap_destroy(sh);
			i3::heap_destroy(h);
		}
		::TlsFree(m_dwTlsIdx); 
	}
	
	void	tls_mgr::OnProcessAttach() 
	{ 
		m_dwTlsIdx = ::TlsAlloc(); 
		// 메인쓰레드에 한해서....tls공간을 만들어야함..
		// 프로세스처리시 쓰레드가 없다고 가정..

		heap_handle heapTLS = heap_create_nolock();
		heap_handle safe_heap = heap_create_lfh();

		void* voiddata = heap_malloc(heapTLS, sizeof(thread_data) );
		thread_data* main_thread_slotlist = new (voiddata) thread_data(heapTLS, safe_heap, 0);
		
		m_usingSlotList.push_back( main_thread_slotlist );
		::TlsSetValue(m_dwTlsIdx, main_thread_slotlist );

		const size_t num_tls = m_TlsBaseList.size();

		for ( size_t i = 0 ; i < num_tls ; ++i )
			m_TlsBaseList[i]->reserve_value(0);

	}

	void	tls_mgr::OnThreadAttach() 
	{

		// 락이 필요없다..DllMain에서 처리됨...
		const size_t num_tls = m_TlsBaseList.size();

		if (!num_tls)			// tls변수가 전역으로 로드되지 않았으면 무시한다..
			return;

		if ( m_reservedSlotList.empty() )		
		{
			heap_handle heapTLS = heap_create_nolock();	
			heap_handle safe_heap = heap_create_lfh();

			WORD newIndex = (WORD)m_usingSlotList.size();

			void* voiddata = heap_malloc(heapTLS, sizeof(thread_data));
				
			thread_data* result =	new (voiddata) thread_data(heapTLS, safe_heap, newIndex);		// 새로운 쓰레드핸들을 확보..
			m_usingSlotList.push_back( result );

			::TlsSetValue(m_dwTlsIdx, result );	

			DWORD hthread = result->get_thread_id();
		
			for ( size_t i = 0 ; i < num_tls ; ++i )
				m_TlsBaseList[i]->reserve_value(hthread);
		}
		else
		{
			thread_data* result = m_reservedSlotList.back();
			::TlsSetValue(m_dwTlsIdx, result );	
			m_reservedSlotList.pop_back();
		}
	}


	// 디태치 되어도 내부 데이터를 삭제하지 않는다...(재활용 대기)
	void	tls_mgr::OnThreadDetach()
	{
		thread_data* data = reinterpret_cast<thread_data*>(::TlsGetValue(m_dwTlsIdx));
		
		if (data)				// 엄한 쓰레드라면 무시하기로 처리..
			m_reservedSlotList.push_back(data);
	}	
		
	void	tls_mgr::OnProcessDetach() 
	{ 

	}
	

	
	}
}