#include <stdio.h>
#include <Windows.h>
#include "S2MO.h"

struct StructTest
{
	INT32								_Test1;
	INT32								_Test2;
};

struct PACKET_TEST : public S2MOPacketBaseT<8282>
{
	S2MOValue<int>						m_TestValue;	
	S2MOStringW<5>						m_TestString;
	S2MOValue<int, 3>					m_TestArray;
	S2MOValue<StructTest>				m_TestStruct;

	PACKET_TEST()
	{
		// Last Update : 2015-10-23 14:25:11
		_SetValue( m_TestValue );
		_SetValue( m_TestString );
		_SetValue( m_TestArray );
		_SetValue( m_TestStruct );
	}
};

int Function( PACKET_TEST& d )
{
	return 0;
}

int main()
{
	char pBuffer[ 8192 ];

	PACKET_TEST Test;

	// Value
	Test.m_TestValue					= 12345;
	int nValue							= Test.m_TestValue;
	// String
	Test.m_TestString					= L"1234";

	// Array
	// 1. 
	Test.m_TestArray[0]					= 1;
	Test.m_TestArray[1]					= 2;
	Test.m_TestArray[2]					= 3;
	// 2. 
	INT32 i32Array[ 3 ] = { 1, 2, 3 };
	Test.m_TestArray.SetValue( i32Array, 0, 3 );
	// 3.
	memcpy( &Test.m_TestArray, i32Array, sizeof(INT32)*3 );
	Test.m_TestArray.SetCount( 3 );

	memcpy( i32Array, &Test.m_TestArray, sizeof(INT32)*3 );

	INT32 i32Count;
	Test.m_TestArray.GetValue( i32Array, 3, &i32Count );

	// Struct
	// 1. 
	(&Test.m_TestStruct)->_Test1		= 123;
	(&Test.m_TestStruct)->_Test2		= 1233;
	// 2.
	StructTest* pStructValue = &Test.m_TestStruct;
	pStructValue->_Test1				= 123;
	pStructValue->_Test2				= 456;
	// 3.
	StructTest StructValue;
	StructValue._Test1					= 123;
	StructValue._Test2					= 456;
	Test.m_TestStruct					= StructValue;
	
	Function( Test );

	PACKET_TEST TestCopy;
	TestCopy = Test;

	S2MOMakePacket( &Test, pBuffer );

	PACKET_TEST RecvTest;
	S2MORecvPacket( &RecvTest, pBuffer );

	return 0;
}