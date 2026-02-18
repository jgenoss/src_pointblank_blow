#include "i3CommonType.h"

#include "pool/pool_by_thread.h"

#include "pool/align_up_down.h"
#include "memory/calculate_adjusted_size.h"
#include "memory/calculate_adjusted_align.h"
#include "pool/static_large_object_cache.h"


namespace i3
{
	const size_t	init_pool_memory_size = 1024 * 1024;		// 1M으로 확정


	const size_t	num_pool_by_size = static_convert_size_to_size_idx<maximum_object_size>::value + 1;

	namespace detail
	{

#pragma warning (disable : 4073)
#pragma init_seg(lib)
		class convert_size_idx_to_actual_size_table
		{
		public:
			convert_size_idx_to_actual_size_table()
			{
				for (size_t i = 0 ; i < num_pool_by_size; ++i )
					m_table[i] = convert_size_idx_to_actual_size(i);
			}
			inline size_t		operator[](size_t i) const { return m_table[i]; }

			~convert_size_idx_to_actual_size_table(){}
		private:
			size_t	m_table[num_pool_by_size];
		} const g_actual_size_table;
		
	}
	__forceinline
	size_t		convert_size_idx_to_actual_size_with_table(size_t i)
	{
		return detail::g_actual_size_table[i];
	}
	
		
	pool_by_thread::pool_by_thread() : pool_tls_base<pool_by_thread>( init_pool_memory_size ), 
		m_ap_pool_by_size(NULL), m_ap_free_block_by_size(NULL)
	{

	}

	pool_by_thread::~pool_by_thread()
	{
		DWORD threadID = shared_tls_type::get_thread_id();
		i3::tls_aligned_free(threadID, m_ap_free_block_by_size);
		i3::tls_free( threadID, m_ap_pool_by_size );
	}

	void pool_by_thread::on_create_tls()
	{
		DWORD threadID = shared_tls_type::get_thread_id();

		m_ap_pool_by_size = reinterpret_cast<pool_by_size*>(i3::tls_malloc( threadID, num_pool_by_size * sizeof(pool_by_size)));
		::memset(m_ap_pool_by_size, 0,  num_pool_by_size * sizeof(pool_by_size));

		m_ap_free_block_by_size = reinterpret_cast<tss_free_block_by_size*>(i3::tls_aligned_malloc(threadID, num_pool_by_size * sizeof(tss_free_block_by_size), 
			pool_block_header_alignment ));
		::memset(m_ap_free_block_by_size, 0, num_pool_by_size * sizeof(tss_free_block_by_size));

		if (threadID == 0)		// 메인쓰레드에 한해서 메모리를 예약..
		{
			expand_memory_size_by_pow2(2);			// 4메가..
			malloc_raw_memory(threadID);			// 지연 생성 목적..
		}
	}
	

	void*				pool_by_thread::allocate( size_t size, size_t align)
	{
		size = align_up(calculate_adjusted_size(size), calculate_adjusted_align(align));		// 이후로는 alignment요구사항없음..

		if ( size >  maximum_object_size )		// 이 경우, 일반적으로 처리...
			return i3::static_large_object_cache::i()->allocate(size);
//			return i3::tls_safe_aligned_malloc( i3::shared_tls<pool_by_thread>::get_thread_id(), size, align );		// 

		size_t size_index = convert_size_to_size_idx(size);
		void* result;

		mem_block_header*& ref_active_block = m_ap_pool_by_size[size_index].m_active_block;

		if ((result = detail::allocate_from_active_blocks( ref_active_block )) != NULL )
			return result;

		//  active_block == NULL임.. (모든 블록의 prev/next 링크가 모두 깨져있어야함..)  tss의 프리 리스트를 뒤져볼것.!!
		if ((result = detail::allocate_from_tss_blocks(ref_active_block, &m_ap_free_block_by_size[size_index] )) != NULL )
			return result;
		// tls쪽 처리로도 되지 않는 경우... (새로운 블록을 할당받아야함)
		return pool_base::allocate_from_new_block(	i3::shared_tls<pool_by_thread>::get_thread_id(), 
			ref_active_block, size_index, convert_size_idx_to_actual_size_with_table( size_index) );
	}


	inline
	void			pool_by_thread::deallocate_other_thread(mem_block_header* block, void* data)
	{
		detail::deallocate_other_thread( &m_ap_free_block_by_size[block->size_index], block, data );
	}

	void			pool_by_thread::deallocate( void* data)
	{
//		널값비교를 좀더 상위에 두도록 변경...if (!data) return;
		
		if ( i3::is_align( data, pool_block_size) )		// large object를 이제 16K에 정렬시킴..
		{
			i3::static_large_object_cache::i()->deallocate(data);	return;		// 라지오브젝트쪽으로 돌림..
		}
		
		mem_block_header* block = reinterpret_cast<mem_block_header*>(align_down( data, pool_block_size) );

		DWORD threadID = i3::shared_tls<pool_by_thread>::get_thread_id();
		
		// 여기서부터는 쓰레드에 의해 갈림..
		if ( block->threadID == threadID )		// 동일 쓰레드..
			deallocate_same_thread( m_ap_pool_by_size[block->size_index].m_active_block, block, data);
		else
			get_by_thread_id( block->threadID )->deallocate_other_thread(  block, data  );	// 타쓰레드....pool_by_thread의 다른 인스턴스에 호출한다..
	}







	
	
	
}