#include "i3CommonType.h"
#include "i3Base/pool/pool_size_helper.h"

namespace i3
{
	//  수정1 ( 32 -> 16 (16*4 = 64or128), 7(128승수) -> 6(64승수) )	: 64(or128)바이트까지는 4(or8)바이트씩 증가..
	//  수정2 ( 16 -> 4 (분할수), 4 -> 2(분할수의 승수) )				: 16개분할에서 4개분할로 감소시킴..
	//  현재 방식 : 64(128)까지 4(8)배수 증가 / 이후, 4개분할 방식..
	//

	size_t		convert_size_to_size_idx( size_t size)
	{
		if (size <=  sizeof(void*) *  16 )					
			return	( size > 0 ) ? ( (size - 1) >> align_index_shift) : 0;			

		// 2의 승수마다 16개로 분할..
		size_t MSB = size_t(i3::msb32(DWORD(size) - 1));					
		size_t base   = 16 + 4 * ( MSB - 6 - sizeof(void*)/8 );
		size_t offset = ((size - 1) >> (MSB-2)) - 4;			
		return base + offset;
	}

	size_t		convert_size_to_actual_size( size_t size)
	{
		if ( size <= sizeof(void*) * 16 )
			return	( size > 0 ) ? ( (size + sizeof(void*)-1) & ~(sizeof(void*)-1) ) : sizeof(void*);		// align_up

		size_t MSB  = size_t(i3::msb32(DWORD(size) - 1));
		size_t offset = (( size - 1) >> (MSB - 2)) - 4;		// 옵셋인덱스..를 다시 환원...
		return	(1 << MSB) + ((offset + 1) << (MSB - 2));
	}

	size_t		convert_size_idx_to_actual_size(size_t size_index)
	{
		if ( size_index < 16 )
			return (size_index + 1) << align_index_shift;
		// 2의 승수 고려..
		size_index -= 16;
		size_t div = size_index / 4;
		size_t offset = size_index + 1 - div * 4 ;		// 나머지가 옵셋에 해당..
		size_t MSB = div + 6 + sizeof(void*)/8;		
		return  ( 1 << MSB ) + (offset << (MSB - 2)) ;
	}
	
}
