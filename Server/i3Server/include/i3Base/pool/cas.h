#pragma once

#pragma warning(push)
#pragma warning(disable : 4995 )
#include <intrin.h>
#pragma warning(pop)

#include <windows.h>

namespace i3
{

	__forceinline bool	cas( void* volatile * ppData, void* pOldData, void* pNewData)
	{
		return InterlockedCompareExchangePointer( ppData, pNewData, pOldData) == pOldData;
	}

//     return((PVOID)(LONG_PTR)InterlockedCompareExchange((LONG volatile *)Destination, (LONG)(LONG_PTR)ExChange, (LONG)(LONG_PTR)Comperand));	


#ifndef _WIN64
	
extern "C" __int64 __cdecl _InterlockedCompareExchange64(__int64 volatile * Destination, __int64 Exchange, __int64 Comperand);

#pragma intrinsic (_InterlockedCompareExchange64)

	// ppData의 경우 인접데이터로 volatile한 size_t가 필요함..
	// 64비트OS와의 호환성을 위해서는 *ppData가 16바이트 정렬을 가질 필요가 있다...
	__forceinline bool cas2( void* volatile* ppData, void* pOldData, size_t OldInt, void* pNewData, size_t NewInt)
	{
		__int64	Comperand = reinterpret_cast<__int64>(pOldData) | (static_cast<__int64>(OldInt) << 32);
		__int64 Exchange  = reinterpret_cast<__int64>(pNewData) | (static_cast<__int64>(NewInt) << 32);
		return _InterlockedCompareExchange64(reinterpret_cast<__int64 volatile *>(ppData), Exchange, Comperand) == Comperand;
	}

#else
	
extern "C" unsigned char __cdecl _InterlockedCompareExchange128(__int64 volatile * Destination,	__int64 ExchangeHigh,
		__int64 ExchangeLow, __int64 * ComparandResult );

#pragma intrinsic (_InterlockedCompareExchange128)


	__forceinline bool cas2( void* volatile* ppData, void* pOldData, size_t OldInt, void* pNewData, size_t NewInt)
	{
		__int64 Comperand[2] = {  reinterpret_cast<__int64>(pOldData), static_cast<__int64>(OldInt)    };
		return _InterlockedCompareExchange128(reinterpret_cast<__int64 volatile*>(ppData), 
			reinterpret_cast<__int64>(pNewData), static_cast<__int64>(NewInt), Comperand) != 0;
	}
		
#endif

	__forceinline bool cas_long( long volatile* data, long old_data, long new_data )
	{
		return ::InterlockedCompareExchange(data, new_data, old_data ) == old_data;
	}
	
	
}