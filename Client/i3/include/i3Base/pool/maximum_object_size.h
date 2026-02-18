#pragma once

#include "mem_block_header.h"
#include "pool_size_helper.h"

namespace i3
{

	namespace detail
	{
		template<size_t object_per_block, size_t size_index, bool match>
		struct static_calculate_maximum_object_size_idx_helper
		{
			static const size_t value = size_index;			
		};

		template<size_t object_per_block, size_t size_index>
		struct static_calculate_maximum_object_size_idx_helper<object_per_block, size_index, false>
		{
		private:
			static const size_t object_total_size = 
				static_convert_size_idx_to_actual_size<size_index - 1>::value * object_per_block;

		public:
			static const size_t value = 
				static_calculate_maximum_object_size_idx_helper
				<
				object_per_block, size_index - 1, 
				( object_total_size < pool_block_size - sizeof(mem_block_header) ) 
				>::value;
		};
	}


	template<size_t object_per_block>
	struct static_calculate_maximum_object_size
	{
	private:
		static const size_t result_temp = ( (pool_block_size - sizeof(mem_block_header) ) / object_per_block ) & ~(128 - 1 );
		static const size_t index_temp = static_convert_size_to_size_idx<result_temp>::value;
		static const size_t actual_total_size_temp = static_convert_size_idx_to_actual_size<index_temp>::value * object_per_block;

		static const size_t size_index = 
			detail::static_calculate_maximum_object_size_idx_helper
			<
			object_per_block, index_temp, ( actual_total_size_temp < pool_block_size - sizeof(mem_block_header) )
			>::value;
	public:
		static const size_t value = static_convert_size_idx_to_actual_size<size_index>::value;		
	};

	const size_t	maximum_object_size = static_calculate_maximum_object_size<1>::value;	// 1개라도 적당하면 무조건 넣도록 한다..
	//static_calculate_maximum_object_size<2>::value;

	//	const size_t	maximum_object_size = ( (pool_block_size - sizeof(mem_block_header) ) /2) & ~(128 - 1);	// 8064바이트..




}