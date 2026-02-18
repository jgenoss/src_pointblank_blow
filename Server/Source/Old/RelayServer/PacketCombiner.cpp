#include "pch.h"
#include "PacketCombiner.h"
#include "LogFile.h"

I3_CLASS_INSTANCE(CPacketCombiner, i3ElementBase);

CPacketCombiner::CPacketCombiner()
{
	m_pBuffer = NULL;
	m_nSize = 0;
	m_nFront = 0;
	m_nRear = 0;
	m_nPacketLength = 0;
}

CPacketCombiner::~CPacketCombiner()
{
	I3MEM_SAFE_FREE(m_pBuffer);
}

/// 생성
/// param nSize		패킷버퍼의 크기
void CPacketCombiner::Create(UINT32 nSize)
{
	I3ASSERT(NULL == m_pBuffer);

	m_pBuffer = (UINT8*)i3MemAlloc(nSize);
	m_nSize = nSize;
	m_nFront = 0;
	m_nRear = 0;
	m_nPacketLength = 0;
}

/// 버퍼에 패킷 쓰기
/// param pData		패킷
/// param nLenght	길이
/// return			성공여부
BOOL CPacketCombiner::PushStream(UINT8* pData,INT32 nLength)
{
	I3ASSERT(m_pBuffer);

	// 여유공간없음
	if ((m_nPacketLength + nLength) > m_nSize)
	{
		return FALSE;
	}

	// 한번에 복사 가능 여부 확인
	if ((m_nRear + nLength) <= m_nSize)
	{
		i3mem::Copy(m_pBuffer + m_nRear, pData, nLength);
	}
	else
	{
		INT32 nFrontLength = m_nSize - m_nRear;
		INT32 nRearLenght = nLength - (m_nSize - m_nRear);

		i3mem::Copy(m_pBuffer + m_nRear, pData, nFrontLength);
		i3mem::Copy(m_pBuffer, pData + nFrontLength, nRearLenght);
	}

	m_nRear = (m_nRear + nLength) % m_nSize;
	m_nPacketLength += nLength;

	return TRUE;
}

/// 완성된 패킷 하나를 받는다
/// param pData		받을 패킷 포인터
/// param bI3Packet	패킷타입
/// return			패킷크기
///					완성된 패킷이 없다면 0
UINT32 CPacketCombiner::PopPacket(UINT8* pData,BOOL bI3Packet)
{
	I3ASSERT(m_pBuffer);

	// Packet 최소 size
	if (2 >= m_nPacketLength)
	{
		return 0;
	}

	// Packet Length - 배열의 양끝에 나뉘어 있는 경우도 고려한다
	INT16 nLength = MAKEWORD(m_pBuffer[m_nFront], m_pBuffer[(m_nFront + 1) % m_nSize]);

	// Encripted i3NetworkPacket의 경우 올바른 크기를 구한다
	if (bI3Packet)
	{
		nLength = ( nLength & 0x7FFF ) + PACKETHEADERSIZE;
	}

	// 아직 완전한 Packet을 받지 못했다
	if (nLength > m_nPacketLength)
	{
		return 0;
	}

	if (nLength > PACKETBUFFERSIZE)
	{
		//WRITE_LOG("PopPacket ASSERT!");
	}

	// 한번에 복사 가능 여부 확인
	if ((m_nFront + nLength) <= m_nSize)
	{
		i3mem::Copy(pData, m_pBuffer + m_nFront, nLength);
	}
	else
	{
		INT32 nFrontLength = m_nSize - m_nFront;
		INT32 nRearLength = nLength - (m_nSize - m_nFront);

		i3mem::Copy(pData, m_pBuffer + m_nFront, nFrontLength);
		i3mem::Copy(pData + nFrontLength, m_pBuffer, nRearLength);
	}

	m_nFront = (m_nFront + nLength) % m_nSize;
	m_nPacketLength -= nLength;

	return nLength;
}

/// 패킷 버퍼 초기화
void CPacketCombiner::Clear(void)
{	
	I3ASSERT(m_pBuffer);

	m_nFront = 0;
	m_nRear = 0;
	m_nPacketLength = 0;	
}
