#include "pch.h"
#include "S2MODataType.h"

BOOL S2MOPackable::_PackingSize( char* pBuffer, int i32Count, int i32MaxSize, int& i32Size )
{
	if( i32MaxSize < 0xFF )
	{
		unsigned char* pTemp = (unsigned char*)pBuffer;
		*pTemp = (unsigned char)i32Count;
		
		i32Size = sizeof(char);
	}
	else if( i32MaxSize < 0xFFFF )
	{
		unsigned short* pTemp = (unsigned short*)pBuffer;
		*pTemp = (unsigned short)i32Count;

		i32Size = sizeof(short);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL S2MOPackable::_UnPackingSize( char* pBuffer, int& i32Count, int i32MaxSize, int& i32Size )
{
	if( i32MaxSize < 0xFF )
	{
		unsigned char ui8Size = *((unsigned char*)pBuffer);
		i32Count = ui8Size;

		i32Size = sizeof(char);
		
	}
	else if( i32MaxSize < 0xFFFF )
	{
		unsigned short ui16Size = *((unsigned short*)pBuffer);
		i32Count = ui16Size;

		i32Size = sizeof(short);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}