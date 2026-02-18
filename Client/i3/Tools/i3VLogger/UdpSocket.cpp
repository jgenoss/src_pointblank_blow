// UdpSocket.cpp : implementation file
//

#include "stdafx.h"
#include "i3VLogger.h"
#include "UdpSocket.h"
#include ".\udpsocket.h"


// CUdpSocket

CUdpSocket::CUdpSocket()
{
	m_pFramework = NULL;
	m_Index = 0;
	m_idxRead = 0;
}

CUdpSocket::~CUdpSocket()
{
}


// CUdpSocket member functions
INT32 CUdpSocket::_FindPattern( INT32 idxStart, char * pszPattern, INT32 idxMax)
{
	INT32 i;
	INT32 idxFind = -1;
	char * pTemp = pszPattern;

	for( i = idxStart; i < idxMax; i++)
	{
		if( m_Buff[i] == *pTemp)
		{
			if( pTemp == pszPattern)
			{
				// 처음 찾아진 경우
				idxFind = i;
			}

			pTemp++;

			if( *pTemp == 0)
			{
				// 완전히 일치하는 패턴이 찾아졌으므로...
				return idxFind;
			}
		}
		else
		{
			// 찾기 초기화
			idxFind = -1;
			pTemp = pszPattern;
		}
	}

	return -1;
}

static INT32 s_TriCount = 0;

void CUdpSocket::ParseQue(void)
{
	while( m_idxRead < m_Index)
	{
		if(	(m_Buff[ m_idxRead]		== 'V') &&
			(m_Buff[ m_idxRead + 1]	== 'L') &&
			(m_Buff[ m_idxRead + 2]	== 'G'))
		{
			// 정상적인 Header
			I3VLOG_HEADER * pHeader = (I3VLOG_HEADER *) _getReadData( sizeof(I3VLOG_HEADER));

			if( (m_Index - m_idxRead) >= pHeader->size)
			{
				UINT32 packetSz = sizeof(I3VLOG_HEADER) + pHeader->size;

				// 정상적인 Packet으로 간주 처리...
				m_pFramework->PushCmd( pHeader, packetSz);

				if( pHeader->tag[3] == 'T')
				{
					s_TriCount++;
					//I3TRACE( "%d\n", s_TriCount);
				}
				else if( pHeader->tag[3] == 'B')
				{
					s_TriCount = 0;
				}

				m_idxRead += pHeader->size;
			}
			else
			{
				// 아직 충분히 Data가 쌓이지 않은 상황
				// 종료하고 나머지 데이터가 마저 전송되기를 기다린다.
				break;
			}
		}
		else
		{
			// 예외적인 상황
			// VGL Sequence를 찾는다.
			m_idxRead = _FindPattern( m_idxRead, "VLG", m_Index);

			if( m_idxRead == -1)
			{
				// 완전히 쓰레기들만 쌓였다고 간주...
				// Reset
				m_idxRead = 0;
				m_Index = 0;
			}
		}
	}

	if( m_idxRead >= m_Index)
	{
		// Reset
		m_idxRead = m_Index = 0;
	}
}

void CUdpSocket::OnReceive(int nErrorCode)
{
	int nRead;

	nRead = Receive( getQueBuffer(), getQueSize());

	switch( nRead)
	{
		case 0:
			Close();
			return;

		case SOCKET_ERROR:
			if (GetLastError() != WSAEWOULDBLOCK) 
			{
				AfxMessageBox ("Error occurred");
				Close();
			}
			return;
	}

	m_Index += nRead;

	// 정상적인 경우
	// Parsing
	//
	// Header의 ID인 VLG 패턴을 찾는다.
	ParseQue();

	CAsyncSocket::OnReceive(nErrorCode);
}
