#include "pch.h"
#include "ProfileDef.h"


#if !defined( I3_NO_PROFILE )

namespace NETPROFILE
{
	void	UDPReset(NETPROFILE::info_t* out_info)
	{
		out_info->m_nRecvCount = out_info->m_nRecvBytes	= out_info->m_nSendCount = out_info->m_nSendBytes = 0;
		out_info->m_fCountTimer = out_info->m_PacketRecvTime = 0.f;
		out_info->m_nInvalidIndex = out_info->m_nWrongPacket = out_info->m_nInvalidAddr = out_info->m_nLostTime = 0;
		out_info->m_fProfileTime = 0.f;
		out_info->m_nTotalRecvCount = out_info->m_nTotalSendCount = out_info->m_nTotalRecvBytes = out_info->m_nTotalSendBytes = 0;
		out_info->m_nRecvCountPerSec = out_info->m_nSendCountPerSec	= out_info->m_nRecvBytesPerSec = out_info->m_nSendBytesPerSec = 0;
		out_info->m_nRecvBytesPerOne = out_info->m_nSendBytesPerOne = out_info->m_nRecvBytesMax = out_info->m_nSendBytesMax = 0;
		out_info->m_nRecvBytesMin = out_info->m_nSendBytesMin = 1000;
	}

	void	TCPReset(NETPROFILE::info_t* out_info)	
	{
		out_info->m_nTcpRecvIdx = out_info->m_nTcpSendIdx = 0;
		for( INT32 i = 0; i < MAX_TCP_MESSAGE_COUNT; i++ )
		{
			out_info->m_nTcpRecvMessage[ i ] = out_info->m_nTcpSendMessage[ i ] = 0;
		}
	}

	void	Reset(NETPROFILE::info_t* out_info) 
	{ 
		UDPReset(out_info); TCPReset(out_info); 
	}

	void	DumpTcpProtocol(const NETPROFILE::info_t* in_info)
	{
		INT32 StartIdx;

		//Recv Buffer
		StartIdx = in_info->m_nTcpRecvIdx;

		for(INT32 i = 0; i < MAX_TCP_MESSAGE_COUNT; i++)
		{
			if( in_info->m_nTcpRecvMessage[StartIdx] != 0)
			{
				I3TRACE("Recv TCP: %d", in_info->m_nTcpRecvMessage[StartIdx]);
			}
			StartIdx--;
			if(StartIdx < 0) StartIdx = MAX_TCP_MESSAGE_COUNT - 1;
		}

		//Send Buffer
		StartIdx = in_info->m_nTcpSendIdx;
		for(INT32 i = 0; i < MAX_TCP_MESSAGE_COUNT; i++)
		{
			if( in_info->m_nTcpSendMessage[StartIdx] != 0)
			{
				I3TRACE("Send TCP: %d", in_info->m_nTcpSendMessage[StartIdx]);
			}
			StartIdx--;
			if(StartIdx < 0) StartIdx = MAX_TCP_MESSAGE_COUNT - 1;
		}
		return;
	}
	
}

#endif