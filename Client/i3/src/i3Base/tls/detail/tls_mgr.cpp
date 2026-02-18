#include "i3CommonType.h"

#include <iostream>
#include <functional>

#include "tls/detail/tls_mgr.h"
#include "tls/tls_ptr.h"

namespace i3
{
	namespace detail_tls 
	{
	
	tls_mgr::tls_mgr() : m_dwTlsIdx(0), m_num_tls(0), 
		m_thread_data_list( heap_allocator< thread_data*>( i3::get_process_heap() ) ),
		m_reserved_td_list(heap_allocator< thread_data*>( i3::get_process_heap() ) ),
		m_reserved_slot_list( heap_allocator<unsigned long>(i3::get_process_heap() ) )
	{
		m_dwTlsIdx = ::TlsAlloc(); 
		// 메인쓰레드에 한해서....tls공간을 만들어야함..
		// 프로세스처리시 쓰레드가 없다고 가정..

		heap_handle heapTLS = heap_create_nolock();
		heap_handle safe_heap = heap_create_lfh();

		void* voiddata = heap_malloc(heapTLS, sizeof(thread_data));
		NetworkDump_Malloc( voiddata, sizeof(thread_data), __FILE__, __LINE__);
		thread_data* main_thread_slotlist = new (voiddata) thread_data(heapTLS, safe_heap, 0);

		m_thread_data_list.push_back( main_thread_slotlist );
		::TlsSetValue(m_dwTlsIdx, main_thread_slotlist );

	}

	tls_mgr::~tls_mgr() 
	{
		// 슬롯 내의 모든 tls데이터를 응용프로그램 종료시에 삭제한다..(그전까지는 삭제가 없었음)
		// 슬롯리스트들에 대해서 모두 Cleanup처리..

		const size_t num_slotlist = m_thread_data_list.size();
		for ( size_t i = 0 ; i < num_slotlist ; ++i )
		{
			thread_data* data = m_thread_data_list[i];
			heap_handle h = data->heapTls;
			heap_handle sh = data->safe_heap;
			data->~thread_data();
			NetworkDump_Free(data, __FILE__, __LINE__);
			i3::heap_free(h, data);
			i3::heap_destroy(sh);
			i3::heap_destroy(h);
		}

		// m_reserved_td_list는 실제로 지울 필요가 없다..이미 m_thread_data_list가 고정되있고, 그곳에 모두 존재한다..
		::TlsFree(m_dwTlsIdx); 
	}
	
	void	tls_mgr::OnProcessAttach() 
	{ 
	

	}

	void	tls_mgr::OnThreadAttach() 
	{

		// 락이 필요없다..DllMain에서 처리됨...
		if (!m_num_tls)			// tls변수가 전역으로 로드되지 않았으면 무시한다..
			return;

		if ( m_reserved_td_list.empty() )		
		{
			heap_handle heapTLS = heap_create_nolock();	
			heap_handle safe_heap = heap_create_lfh();

			WORD newIndex = (WORD)m_thread_data_list.size();

			void* voiddata = heap_malloc(heapTLS, sizeof(thread_data));
			NetworkDump_Malloc( voiddata, sizeof(thread_data), __FILE__, __LINE__);
				
			thread_data* result =	new (voiddata) thread_data(heapTLS, safe_heap, newIndex);		// 새로운 쓰레드핸들을 확보..
			m_thread_data_list.push_back( result );

			::TlsSetValue(m_dwTlsIdx, result );	

		}
		else
		{
			thread_data* result = m_reserved_td_list.back();
			::TlsSetValue(m_dwTlsIdx, result );	
			m_reserved_td_list.pop_back();
		}
	}

	// 디태치 되어도 내부 데이터를 삭제하지 않는다...(재활용 대기)
	void	tls_mgr::OnThreadDetach()
	{
		thread_data* data = reinterpret_cast<thread_data*>(::TlsGetValue(m_dwTlsIdx));
		
		if (data)				// 엄한 쓰레드라면 무시하기로 처리..
			m_reserved_td_list.push_back(data);
	}	
		
	void	tls_mgr::OnProcessDetach() 
	{ 

	}
	
	unsigned long	tls_mgr::obtain_tls_idx()
	{
		i3::scoped_cs l(m_slot_list_cs);

		if ( m_reserved_slot_list.empty() ) return m_num_tls++;
		unsigned long res =	m_reserved_slot_list.back();
		m_reserved_slot_list.pop_back();

		return res;
	}

	void			tls_mgr::return_tls_idx(unsigned long slot)
	{
		i3::scoped_cs l(m_slot_list_cs);
		m_reserved_slot_list.push_back(slot);
	}
	
	
	thread_data::thread_data( heap_handle heapTls, heap_handle safe_heap, DWORD threadID) : 
	slotlist( heap_allocator<void*>( heapTls) ), //fn_list( heap_allocator<fn_value_list>(heapTls) ),
	heapTls(heapTls), safe_heap(safe_heap), hthread(threadID) 
	{

	}

	void		resize_slot(thread_data* td, size_t slot) 
	{
		const size_t old_num = td->slotlist.size();
		td->slotlist.insert(td->slotlist.end(), slot + 1 - old_num, 0);
	}
	

	}
}