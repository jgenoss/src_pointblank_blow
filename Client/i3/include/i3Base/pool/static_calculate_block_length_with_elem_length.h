#pragma once

//
// 네이밍 상 모호성이 있어 반드시 설명..
// 필요로 하는 원소 갯수 대비, 블록 내 원소 용적을 계산하여, 블록의 갯수를 구하는 루틴이다...
//

namespace i3
{
	
	template< UINT block_capacity, UINT required_elem_length>
	struct static_calculate_block_length_with_elem_length
	{
		static const UINT value = ( required_elem_length + block_capacity - 1 ) / block_capacity ;
	};
	
	template< UINT required_elem_length>
	struct static_calculate_block_length_with_elem_length< 0, required_elem_length>
	{
		static const UINT value = 0 ;
	};
		
}
