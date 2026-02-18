#pragma once

#pragma intrinsic(_BitScanReverse)

namespace i3
{
	__forceinline
	bool	msb32( DWORD& out, DWORD n)	{	return	_BitScanReverse(&out, n) != 0;	}
	
	__forceinline
	DWORD	msb32( DWORD val) {	DWORD result;	 _BitScanReverse(&result, val);	return result;  }
	
}
