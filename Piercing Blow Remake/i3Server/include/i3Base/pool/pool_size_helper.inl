

namespace i3
{
	
	const size_t align_index_shift = 2 + (sizeof(void*) >> 3);


	//  수정1 ( 32 -> 16 (16*4 = 64or128), 7(128승수) -> 6(64승수) )	: 64(or128)바이트까지는 4(or8)바이트씩 증가..
	//  수정2 ( 16 -> 4 (분할수), 4 -> 2(분할수의 승수) )				: 16개분할에서 4개분할로 감소시킴..
	//  현재 방식 : 64(128)까지 4(8)배수 증가 / 이후, 4개분할 방식..
	//

	__forceinline size_t		convert_size_to_size_idx( size_t aligned_size)
	{
		if (aligned_size <=  sizeof(void*) *  16 )					
			return ( aligned_size - 1) >> align_index_shift;			
		// 2의 승수마다 16개로 분할..
		size_t MSB = size_t(i3::msb32(DWORD(aligned_size) - 1));					
		size_t base   = 16 + 4 * ( MSB - 6 - sizeof(void*)/8 );
		size_t offset = ((aligned_size - 1) >> (MSB-2)) - 4;			
		return base + offset;
	}

	__forceinline size_t		convert_size_to_actual_size( size_t aligned_size)
	{
		if ( aligned_size <= sizeof(void*) * 16 )
			return  aligned_size;

		size_t MSB  = size_t(i3::msb32(DWORD(aligned_size) - 1));
		size_t offset = (( aligned_size - 1) >> (MSB - 2)) - 4;		// 옵셋인덱스..를 다시 환원...
		return	((size_t)1 << MSB) + ((offset + 1) << (MSB - 2));
	}

	__forceinline size_t		convert_size_idx_to_actual_size(size_t size_index)
	{
		if ( size_index < 16 )
			return (size_index + 1) << align_index_shift;
		// 2의 승수 고려..
		size_index -= 16;
		size_t div = size_index / 4;
		size_t offset = size_index + 1 - div * 4 ;		// 나머지가 옵셋에 해당..
		size_t MSB = div + 6 + sizeof(void*)/8;		
		return  ( (size_t)1 << MSB ) + (offset << (MSB - 2)) ;
	}



	namespace detail
	{
		template<size_t aligned_size, bool small_part>
		struct static_convert_size_to_size_idx_helper
		{
			static const size_t value = (aligned_size - 1) >> align_index_shift;
		};

		template<size_t aligned_size>
		struct static_convert_size_to_size_idx_helper<aligned_size, false>
		{
		private:
			static const size_t msb    = size_t(i3::static_msb32<aligned_size - 1>::value);
			static const size_t base   = 16 + 4 * ( msb - 6 - sizeof(void*)/8 );
			static const size_t offset = ( (aligned_size - 1) >> (msb - 2) ) - 4;
		public:
			static const size_t value = base + offset;
		};

	}

	template<size_t aligned_size>
	struct static_convert_size_to_size_idx
	{
		static const size_t value = detail::static_convert_size_to_size_idx_helper<aligned_size, aligned_size <= sizeof(void*) * 16 >::value;
	};

	namespace detail
	{
		template<size_t aligned_size, bool small_part>
		struct static_convert_size_to_actual_size_helper 
		{
			static const size_t value = aligned_size;
		};

		template<size_t aligned_size>
		struct static_convert_size_to_actual_size_helper<aligned_size, false>
		{
		private:
			static const size_t msb  = size_t(i3::static_msb32<aligned_size - 1>::value);
			static const size_t offset = (( aligned_size - 1) >> (msb - 2)) - 4;		// 옵셋인덱스..를 다시 환원...
		public:		
			static const size_t value = (1 << msb) + ((offset + 1) << (msb - 2));
		};
	}

	template<size_t aligned_size>	
	struct static_convert_size_to_actual_size
	{
		static const size_t value = 
			detail::static_convert_size_to_actual_size_helper<aligned_size, aligned_size <= sizeof(void*) * 16>::value;
	};

	namespace detail
	{
		template<size_t size_index, bool small_part>
		struct static_convert_size_idx_to_actual_size_helper 
		{
			static const size_t value = (size_index + 1) << align_index_shift;
		};

		template<size_t size_index>
		struct static_convert_size_idx_to_actual_size_helper<size_index, false>
		{
		private:
			static const size_t size_index_mod = size_index - 16;
			static const size_t div = size_index_mod / 4;
			static const size_t offset = size_index_mod + 1 - div * 4 ;		// 나머지가 옵셋에 해당..
			static const size_t msb = div + 6 + sizeof(void*)/8;		// div + 7(8)이 MSB에 해당
		public:
			static const size_t value = ( 1 << msb ) + (offset << (msb - 2)) ;
		};
	}

	template<size_t size_index>
	struct static_convert_size_idx_to_actual_size 
	{
		static const size_t value = detail::static_convert_size_idx_to_actual_size_helper<size_index, (size_index < 16) >::value;
	};
}
