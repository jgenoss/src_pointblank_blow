#pragma once

namespace i3
{
	// 간단하게 아스키코드 수준에서만 따진다.. // 오류는 -1..
	inline int char_to_digit( int c ) {   if ( c < 0x30 || c > 0x39) return -1; return c - 0x30; }
}
