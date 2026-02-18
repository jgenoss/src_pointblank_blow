#include "i3CommonType.h"

//
// 가장 흔한 레퍼런스를 구해서 인터페이스만 바꾸고...붙인다..
// http://www.winapi.co.kr/project/dangeun/dg2/dg121.htm
//

namespace i3
{
	
	I3_EXPORT_BASE bool is_utf8( const char* str, std::size_t len)
	{
		const unsigned char* beg = reinterpret_cast<const unsigned char*>(str);
		const unsigned char* end = beg + len;
		std::size_t  maybe_utf8 = 0;
		static const std::size_t heuristic = 10;		// 10개만 조사해서 통과시킨다..
		
		for ( const unsigned char* ptr = beg ; ptr != end ; ++ptr )
		{
			if ( ( *ptr & 0x80 ) == 0x80 )
			{
				if ((*ptr & 0xe0) == 0xc0 )
				{	
					++ptr;	if ( ptr == end ) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false;

					maybe_utf8 += 2;
					if (maybe_utf8 > heuristic) 
						return true;
					continue;
				}
				else
				if ((*ptr & 0xf0) == 0xe0 )
				{
					++ptr;	if (ptr == end) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false; 
					++ptr;	if (ptr == end) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false; 

					maybe_utf8 += 3;
					if (maybe_utf8 > heuristic) 
						return true;
					continue;
				}
				else
				if ((*ptr & 0xf8) == 0xf0 )
				{
					++ptr;	if (ptr == end) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false; 
					++ptr;	if (ptr == end) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false; 
					++ptr;	if (ptr == end) return false;
					if ( (*ptr & 0xc0) != 0x80 ) return false; 

					maybe_utf8 += 4;
					if (maybe_utf8 > heuristic) 
						return true;
					continue;
				}
				return false;
			}

		}

		return  (maybe_utf8 > 0);
	}
	
		
}