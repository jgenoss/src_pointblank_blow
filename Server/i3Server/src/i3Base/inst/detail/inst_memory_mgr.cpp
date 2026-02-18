#include "i3CommonType.h"

#include "inst/detail/inst_memory_mgr.h"

//

namespace i3
{
	namespace detail_inst
	{

		inst_memory_mgr::inst_memory_mgr() : 
			m_inst_map(i3::calculate_bucket_size(64), shared_inst_map::hasher(), shared_inst_map::key_equal(),
				heap_allocator<  shared_inst_map::value_type >(i3::get_process_heap() ) )
		{
			
		}

		inst_memory_mgr::~inst_memory_mgr() 
		{
			
		}
		
	}

}

