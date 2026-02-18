#include "i3CommonType.h"
#include "string/ext/utf8_to_utf16.h"
#include "string/ext/utf16_to_utf8.h"

#include "string/ext/detail/utf_to_utf_impl.h"

//
//  boost locale 코드를 참고하고, 그중 몇가지 루틴은 축소시켜 수정됨..
//
//  utf간 변환은 utf8 --> utf16이 가장 병목부분이다..
//



namespace i3
{
	namespace detail_utf
	{

//      아래 변수값은 utf_to_utf_impl.h 헤더로 이동 되었다.
//		static const uint32_t illegal = 0xFFFFFFFFu;
//		static const uint32_t incomplete = 0xFFFFFFFEu;
//		static const uint32_t utf_error_mask = incomplete;
//		static const uint32_t utf_error_mask_result = incomplete;
		
		inline bool is_valid_codepoint(uint32_t cp)
		{
			return cp <= 0x10FFFF && ( 0xD800 > cp || cp > 0xDFFF );
		}
	}

	namespace detail_utf8
	{
		using namespace i3::detail_utf;


		static const int g_trail_length[] = 
		{
	//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x00
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x10
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x20
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x30
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x40
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x50
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x60
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x70	End of ASCII range
		-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	// 0x80 0x80 to 0xc1 invalid
		-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	// 0x90 
		-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	// 0xa0 
		-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	// 0xb0 
		-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xc0 0xc2 to 0xdf 2 byte
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0xd0
		2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xe0 0xe0 to 0xef 3 byte
		3,	3,	3,	3,	3,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
		};


		inline int trail_length(unsigned char c) {	return g_trail_length[c]; }
		
			
		inline bool is_trail(unsigned char c) { return ( c & 0xC0 ) == 0x80 ; }
		inline bool is_lead(unsigned char c)  { return !is_trail(c); }
		
		uint32_t decode( const char*& pos, const char* end)			// pos != end가 다르도록 사전에 처리한다..
		{
			unsigned char lead = *pos++;							// 참조형을 값으로 해서 넘기는게 컴파일에 좋음..
			int trail_size = trail_length(lead);
			if (trail_size < 0) return illegal;
			if ( pos + trail_size > end ) return incomplete;	// 필요코드보다 적은 양으로 외부버그로 잘린것으로 판단..

			if (trail_size == 0) return uint32_t(lead);						// 영문은 여기서 종료..
			
			uint32_t result = lead & ((1<<(6-trail_size))-1);

			switch(trail_size)
			{
			case 3:	
				if(!is_trail(unsigned char(*pos))) return illegal;
					result = (result << 6) | ( unsigned char(*pos++) & 0x3F);
			case 2:		
				if(!is_trail(unsigned char(*pos))) return illegal;
					result = (result << 6) | ( unsigned char(*pos++) & 0x3F);
			case 1:		
				if(!is_trail(unsigned char(*pos))) return illegal;
					result = (result << 6) | ( unsigned char(*pos++) & 0x3F);
			}
			return result;			
		}
		
		int	encode(uint32_t cp, char(&out)[4])		// 만약 안전하게 하려면 cp적법성을 따져야한다.
		{
			if (cp < 0x80) 
			{
				out[0] = char(cp);	
				return 1;
			}
			else if ( cp < 0x800)
			{
				out[0] = char( (cp >> 6) | 0xC0 ) ;
				out[1] = char( (cp & 0x3F) | 0x80);
				return 2;
			}
			else if ( cp < 0x10000 )
			{
				out[0] = char( (cp >> 12) | 0xE0) ;
				out[1] = char( ( ( cp >> 6) & 0x3F) | 0x80 ) ;
				out[2] = char( ( cp & 0x3F) | 0x80 ) ;
				return 3;
			}
			
			out[0] = char( (cp >> 18) | 0xF0 );
			out[1] = char( ( (cp >> 12) & 0x3F) | 0x80);
			out[2] = char( ( (cp >> 6 ) & 0x3F) | 0x80);
			out[3] = char( (cp & 0x3F ) | 0x80 );
			return 4;
			
		}
	}

	namespace detail_utf16
	{
		using namespace i3::detail_utf;

		inline bool is_1st_surrogate(wchar_t c)
		{
			return 0xD800 <= c && c <= 0xDBFF;
		}
		inline bool is_2nd_surrogate(wchar_t c)
		{
			return 0xDC00 <= c && c <= 0xDFFF;
		}


		inline uint32_t combie_surrogate(wchar_t c1, wchar_t c2)
		{
			return ( ( uint32_t(c1 & 0x3FF) << 10 ) | ( c2 & 0x3FF) ) + 0x10000;
		}
		inline int trail_length(wchar_t c)
		{
			if (is_1st_surrogate(c) ) return 1;
			if (is_2nd_surrogate(c) ) return -1;
			return 0;
		}
		
		uint32_t decode( const wchar_t*& pos, const wchar_t* end)			// pos != end가 다르도록 사전에 처리한다..
		{
			wchar_t c1 = *pos++;
			if ( c1 < 0xD800 || c1 > 0xDFFF ) 
				return uint32_t(c1);				// 대부분 여기서 종료되는 것으로 기대..
			if ( c1 > 0xDBFF) return illegal;
			if (pos == end) return incomplete;

			wchar_t c2 = *pos++;
			if ( c2 < 0xDC00 || 0xDFFF < c2 )
				return illegal;
			return combie_surrogate(c1, c2);
		}
		
		int		encode( uint32_t cp, wchar_t(&out)[2])		// 만약 안전하게 하려면 cp적법성을 따져야한다.
		{
			if ( cp <= 0xFFFF )						// 대부분 여기서 종료되는 것으로 기대..
			{
				out[0] = wchar_t(cp); return 1;
			}
			
			cp -= 0x10000;
			out[0] = wchar_t(0xD800 | ( cp>>10) );
			out[1] = wchar_t(0xDC00 | ( cp & 0x3FF) );
			return 2;
		}
	}
	
	namespace detail
	{
		void utf8_to_utf16_impl(const char* input, std::size_t input_size, i3::wstring& out)
		{
			out.clear();

			if (input_size == 0) 
				return;
			
			// 한글기준 3바이트라면 --> 갯수는 오히려 줄어드는 방향으로 가게되므로, 같은 사이즈를 예약 해두어도 괜찮을 것이다.
			out.reserve(input_size);
			
			const char* beg = input;
			const char* end = input + input_size;
			
			wchar_t     result_c[2];

			while ( beg != end )
			{
				uint32_t cp = detail_utf8::decode(beg, end);
				if (  ( cp & detail_utf::utf_error_mask) != detail_utf::utf_error_mask_result )		// 일단은 에러인경우 쌩깐다..
				{
					const int num_res = detail_utf16::encode(cp, result_c);
					out.append(result_c, num_res);
				}
			}
					
		}

		void utf16_to_utf8_impl(const wchar_t* input, std::size_t input_size, i3::string& out)
		{
			out.clear();
			if (input_size == 0)
				return;

			// 한글기준이라면 갯수는 일반적으로 3배까지 확장될수도 있다..
			out.reserve(input_size * 3);
			
			const wchar_t* beg = input;
			const wchar_t* end = input + input_size;

			char	result_c[4];		

			while( beg != end)
			{
				uint32_t cp = detail_utf16::decode(beg, end);
				if ( ( cp & detail_utf::utf_error_mask) != detail_utf::utf_error_mask_result )		// 일단은 에러인경우 쌩깐다..
				{
					const int num_res = detail_utf8::encode(cp, result_c);
					out.append(result_c, num_res);
				}
			}
		}

	}
	
}

