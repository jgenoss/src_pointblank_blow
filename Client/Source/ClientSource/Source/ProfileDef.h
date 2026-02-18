#if !defined( __PROFILE_DEF_H__)
#define __PROFILE_DEF_H__

#define MAX_TCP_MESSAGE_COUNT		100

#if !defined( I3_NO_PROFILE )

namespace NETPROFILE
{
	struct info_t
	{
		INT32			m_nRecvCount = 0;				// 초당 받은 횟수
		INT32			m_nRecvBytes = 0;				// 초당 받은 패킷량
		INT32			m_nSendCount = 0;				// 초당 보낸 횟수
		INT32			m_nSendBytes = 0;				// 초당 보낸 패킷량
		REAL32			m_fCountTimer = 0.0f;				//
		REAL32			m_PacketRecvTime = 0.0f;			// 패킷을 받는 타임( Debug 용입니다. 이 시간당 패킷을 처리함 )
		INT32			m_nInvalidIndex = 0;			// 잘못된 인덱스가 들어온 경우 횟수
		INT32			m_nWrongPacket = 0;				// 잘못된 패킷데이터인 경우 횟수
		INT32			m_nInvalidAddr = 0;				// 잘못된 주소로부터 받은 경우 횟수
		INT32			m_nLostTime = 0;				// 지연된 패킷을 받은 경우 횟수
		
		REAL32			m_fProfileTime = 0.0f;				// 총 플레이타임
		UINT32			m_nTotalRecvCount = 0;			// 총 RecvCount
		UINT32			m_nTotalSendCount = 0;			// 총 SendCount
		UINT32			m_nTotalRecvBytes = 0;			// 총 RecvBytes
		UINT32			m_nTotalSendBytes = 0;
		UINT32			m_nRecvCountPerSec = 0;			// 평균 초당 RecvCount
		UINT32			m_nSendCountPerSec = 0;
		UINT32			m_nRecvBytesPerSec = 0;			// 평균 초당 RecvBytes
		UINT32			m_nSendBytesPerSec = 0;			// 평균 초당 SendBytes
		UINT32			m_nRecvBytesPerOne = 0;			// 평균 한 패킷당 RecvBytes
		UINT32			m_nSendBytesPerOne = 0;			// 평균 한 패킷당 SendBytes
		INT32			m_nRecvBytesMax = 0;			// 최대 RecvBytes
		INT32			m_nRecvBytesMin = 0;			// 최소 RecvBytes
		INT32			m_nSendBytesMax = 0;			// 최대 SendBytes
		INT32			m_nSendBytesMin = 0;			// 최소 SendBytes

		//For SERVER TCP 	
		PROTOCOL		m_nTcpRecvMessage[MAX_TCP_MESSAGE_COUNT] = { 0 };
		INT32			m_nTcpRecvIdx = 0;
		PROTOCOL		m_nTcpSendMessage[MAX_TCP_MESSAGE_COUNT] = { 0 };
		INT32			m_nTcpSendIdx = 0;
	};

	void	UDPReset(NETPROFILE::info_t* out_info);
	void	TCPReset(NETPROFILE::info_t* out_info);	
	void	Reset(NETPROFILE::info_t* out_info);
	
	void	DumpTcpProtocol(const NETPROFILE::info_t* in_info);

}



#endif

#endif
