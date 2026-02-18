#include "pch.h"
#include "PacketCombiner.h"
//#include "LogFile.h"

I3_CLASS_INSTANCE(CPacketCombiner, i3ElementBase);

CPacketCombiner::CPacketCombiner()
{
	m_pui8Buffer	= NULL;
	m_i32Size		= 0;
	m_i32Front		= 0;
	m_i32Rear		= 0;
	m_i32PacketLength = 0;
}

CPacketCombiner::~CPacketCombiner()
{
	I3MEM_SAFE_FREE(m_pui8Buffer);
}

void CPacketCombiner::Create(UINT32 ui32Size)
{
	I3ASSERT(NULL == m_pui8Buffer);

	m_pui8Buffer		= (UINT8*)i3MemAlloc(ui32Size);
	i3mem::FillZero( m_pui8Buffer, ui32Size );
	m_i32Size			= ui32Size;
	m_i32Front			= 0;
	m_i32Rear			= 0;
	m_i32PacketLength	= 0;
}

BOOL CPacketCombiner::PushStream(UINT8* pui8Data,INT32 i32Length)
{
	I3ASSERT(m_pui8Buffer);

	// 여유공간없음
	if (( m_i32PacketLength + i32Length ) > m_i32Size )
	{
		return FALSE;
	}

	// 한번에 복사 가능 여부 확인
	if( (m_i32Rear + i32Length) <= m_i32Size )
	{
		i3mem::Copy( m_pui8Buffer + m_i32Rear, pui8Data, i32Length );
	}
	else
	{
		INT32 i32FrontLength	= m_i32Size - m_i32Rear;
		INT32 i32RearLenght		= i32Length - i32FrontLength;

		i3mem::Copy(m_pui8Buffer + m_i32Rear, pui8Data, i32FrontLength);
		i3mem::Copy(m_pui8Buffer, pui8Data + i32FrontLength, i32RearLenght);
	}

	m_i32Rear			= (m_i32Rear + i32Length) % m_i32Size;
	m_i32PacketLength	+= i32Length;

	return TRUE;
}

// Packet이 완성되면, 완성된 한 Packet을 돌려준다.
UINT32 CPacketCombiner::PopPacket(UINT8* pui8Data,BOOL bI3Packet)
{
	I3ASSERT(m_pui8Buffer);

	// Packet 최소 size 4
	if( PACKETHEADERSIZE > m_i32PacketLength )
	{
		return 0;
	}

	// Packet Length - 배열의 양끝에 나뉘어 있는 경우도 고려한다
	INT16 i16Length = MAKEWORD(m_pui8Buffer[m_i32Front], m_pui8Buffer[(m_i32Front + 1) % m_i32Size]);

	// Encripted i3NetworkPacket의 경우 올바른 크기를 구한다
	if( bI3Packet )
	{
		i16Length = (i16Length & 0x7FFF) + PACKETHEADERSIZE;
	}

	// 아직 완전한 Packet을 받지 못했다
	if( i16Length > m_i32PacketLength )
	{
		return 0;
	}

	if( i16Length > PACKETBUFFERSIZE )
	{
		Clear();
		// 이경우 패킷이 깨졌다.
		return 0;
	}

	// 한번에 복사 가능 여부 확인
	if( (m_i32Front + i16Length) <= m_i32Size )
	{
		i3mem::Copy( pui8Data, m_pui8Buffer + m_i32Front, i16Length );
	}
	else
	{
		INT32 i32FrontLength	= m_i32Size - m_i32Front;
		INT32 i32RearLength		= i16Length - i32FrontLength;

		i3mem::Copy( pui8Data, m_pui8Buffer + m_i32Front, i32FrontLength );
		i3mem::Copy( pui8Data + i32FrontLength, m_pui8Buffer, i32RearLength );
	}

	m_i32Front		= (m_i32Front + i16Length) % m_i32Size;
	m_i32PacketLength -= i16Length;

	return i16Length;
}

void CPacketCombiner::Clear(void)
{	
	I3ASSERT(m_pui8Buffer);

	m_i32Front		= 0;
	m_i32Rear		= 0;
	m_i32PacketLength = 0;
}
