

namespace i3
{
	
	const size_t align_index_shift = 2 + (sizeof(void*) >> 3);


	//  수정1 ( 32 -> 16 (16*4 = 64or128), 7(128승수) -> 6(64승수) )	: 64(or128)바이트까지는 4(or8)바이트씩 증가..
	//  수정2 ( 16 -> 4 (분할수), 4 -> 2(분할수의 승수) )				: 16개분할에서 4개분할로 감소시킴..
	//  현재 방식 : 64(128)까지 4(8)배수 증가 / 이후, 4개분할 방식..
	//




	namespace detail
	{
		template<size_t size, bool small_part>
		struct static_convert_size_to_size_idx_helper
		{
			static const size_t value =	(size > 0 ) ? ( (size - 1) >> align_index_shift ) : 0;
		};

		template<size_t size>
		struct static_convert_size_to_size_idx_helper<size, false>
		{
		private:
			static const size_t msb    = size_t(i3::static_msb32<size - 1>::value);
			static const size_t base   = 16 + 4 * ( msb - 6 - sizeof(void*)/8 );
			static const size_t offset = ( (size - 1) >> (msb - 2) ) - 4;
		public:
			static const size_t value = base + offset;
		};

	}

	template<size_t size>
	struct static_convert_size_to_size_idx
	{
		static const size_t value = detail::static_convert_size_to_size_idx_helper<size, size <= sizeof(void*) * 16 >::value;
	};

	namespace detail
	{
		template<size_t size, bool small_part>
		struct static_convert_size_to_actual_size_helper 
		{
			static const size_t value = ( size > 0 ) ? ( (size + sizeof(void*)-1) & ~(sizeof(void*)-1) ) : sizeof(void*);		// align_up;
		};

		template<size_t size>
		struct static_convert_size_to_actual_size_helper<size, false>
		{
		private:
			static const size_t msb  = size_t(i3::static_msb32<size - 1>::value);
			static const size_t offset = (( size - 1) >> (msb - 2)) - 4;		// 옵셋인덱스..를 다시 환원...
		public:		
			static const size_t value = (1 << msb) + ((offset + 1) << (msb - 2));
		};
	}

	template<size_t size>	
	struct static_convert_size_to_actual_size
	{
		static const size_t value = 
			detail::static_convert_size_to_actual_size_helper<size, size <= sizeof(void*) * 16>::value;
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
