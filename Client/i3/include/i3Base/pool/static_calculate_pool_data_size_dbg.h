#pragma once

#include "pool_dbg_user_config.h"		// 다른 dbg_util의 헤더가 필요한 것이 아님..
#include "../memory/calculate_adjusted_size.h"


//
// 디버그 유틸에서 메모리풀로부터 메모리를 확보시, 기본적 누수/위반검사를 위한 헤더+더미 데이터가 추가되므로..
// 다음과 같이 구분하도록 처리한다...
// 여기서, 메모리풀은 정렬이 강제되므로, 정렬을 고려하지 않은 메타 함수는 포함시키지 않았다...
//
#if defined(_DEBUG) && defined(I3_POOL_DBG)

#include "../memory/calculate_adjusted_align.h"
#include "pool_dbg_block.h"

namespace i3
{	
	template<size_t size, size_t align>
	struct static_calculate_pool_data_size_dbg
	{
		static const size_t adjusted_size = i3::static_calculate_adjusted_size<size>::value;
		static const size_t adjusted_align = i3::static_calculate_adjusted_align<align>::value;
		static const size_t value = sizeof(i3::dbg_block) + adjusted_size + adjusted_align + sizeof( i3::aligned_dummy);		// + sizeof(DWORD) 
	};
}	

#else

namespace i3
{
	template<size_t size, size_t align>
	struct static_calculate_pool_data_size_dbg
	{
		static const size_t value =   i3::static_calculate_adjusted_size<size>::value;		// 현재 메모리풀 특성상 정렬은 자동화되므로 포함시키지 않는다..
	};
}

#endif


