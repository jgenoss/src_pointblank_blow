#include "pch.h"
#include "PacketCombiner.h"

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

/// 생성
/// param nSize		패킷버퍼의 크기
void CPacketCombiner::Create(UINT32 ui32Size)
{
	I3ASSERT(NULL == m_pui8Buffer);

	m_pui8Buffer = (UINT8*)i3MemAlloc(ui32Size);
	m_i32Size = ui32Size;
	m_i32Front = 0;
	m_i32Rear = 0;
	m_i32PacketLength = 0;
}

/// 버퍼에 패킷 쓰기
/// param pData		패킷
/// param nLenght	길이
/// return			성공여부
BOOL CPacketCombiner::PushStream(UINT8* pui8Data,INT32 i32Length)
{
	I3ASSERT(m_pui8Buffer);

	// 여유공간없음
	if ((m_i32PacketLength + i32Length) > m_i32Size)
	{
		return FALSE;
	}

	// 한번에 복사 가능 여부 확인
	if ((m_i32Rear + i32Length) <= m_i32Size)
	{
		i3mem::Copy(m_pui8Buffer + m_i32Rear, pui8Data, i32Length);
	}
	else
	{
		INT32 i32FrontLength = m_i32Size - m_i32Rear;
		INT32 i32RearLenght = i32Length - (m_i32Size - m_i32Rear);

		i3mem::Copy(m_pui8Buffer + m_i32Rear, pui8Data, i32FrontLength);
		i3mem::Copy(m_pui8Buffer, pui8Data + i32FrontLength, i32RearLenght);
	}

	m_i32Rear = (m_i32Rear + i32Length) % m_i32Size;
	m_i32PacketLength += i32Length;

	return TRUE;
}

/// 완성된 패킷 하나를 받는다
/// param pData		받을 패킷 포인터
/// param bI3Packet	패킷타입
/// return			패킷크기
///					완성된 패킷이 없다면 0
UINT32 CPacketCombiner::PopPacket(UINT8* pui8Data,BOOL bI3Packet)
{
	I3ASSERT(m_pui8Buffer);

	// Packet 최소 size
	if (2 >= m_i32PacketLength)
	{
		return 0;
	}

	// Packet Length - 배열의 양끝에 나뉘어 있는 경우도 고려한다
	INT16 i16Length = MAKEWORD(m_pui8Buffer[m_i32Front], m_pui8Buffer[(m_i32Front + 1) % m_i32Size]);

	// Encripted i3NetworkPacket의 경우 올바른 크기를 구한다
	if (bI3Packet)
	{
		i16Length = UNSETBIT(i16Length, 0x8000) + PACKETHEADERSIZE;
	}

	// 아직 완전한 Packet을 받지 못했다
	if (i16Length > m_i32PacketLength)
	{
		return 0;
	}

	if (i16Length > PACKETBUFFERSIZE)
	{
		// TODO
		// char strTemp[ 1024 ];
		//i3String::Format( strTemp, 1024, "PopPacket ASSERT! Size : %d", nLength );
		//COMMON_LOG( strTemp );
		//쓰레드 확인하고 보내야함.
		Clear();
		// 이경우 패킷이 깨졌다.
		return 0;
	}

	// 한번에 복사 가능 여부 확인
	if ((m_i32Front + i16Length) <= m_i32Size)
	{
		i3mem::Copy(pui8Data, m_pui8Buffer + m_i32Front, i16Length);
	}
	else
	{
		INT32 nFrontLength = m_i32Size - m_i32Front;
		INT32 nRearLength = i16Length - (m_i32Size - m_i32Front);

		i3mem::Copy(pui8Data, m_pui8Buffer + m_i32Front, nFrontLength);
		i3mem::Copy(pui8Data + nFrontLength, m_pui8Buffer, nRearLength);
	}

	m_i32Front = (m_i32Front + i16Length) % m_i32Size;
	m_i32PacketLength -= i16Length;

	return i16Length;
}

/// 패킷 버퍼 초기화
void CPacketCombiner::Clear(void)
{	
	I3ASSERT(m_pui8Buffer);

	m_i32Front = 0;
	m_i32Rear = 0;
	m_i32PacketLength = 0;
}
