#ifndef __PROTOCOL_QUICKJOIN_STRUCT_H__
#define __PROTOCOL_QUICKJOIN_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_START_REQ		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_START_REQ		// 빠른 입장 시작 요청. Client -> Game
{
	UINT8			m_ui8QuickJoinIdx;		// m_aQuickJoinInfo 중 실제로 검색할 조건. 0~MAX_QUICK_JOIN_INFO_COUNT-1 까지 유효. 
	QUICKJOIN_INFO	m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];	// 빠른입장 설정 정보. 전체가 DB에 저장됨.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_START_ACK	 
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_START_ACK		// 빠른 입장 시작 응답. Game -> Client
{
	INT32			m_i32Rv;			// 결과값. EVENT_ERROR_SUCCESS : 성공, EVENT_ERROR_QUICK_JOIN_2ND_BEST : 차선책 권유, 그외 : 실패
	QUICKJOIN_INFO	m_aQuickJoinInfo[MAX_QUICK_JOIN_INFO_COUNT];	// 빠른입장 설정 정보.

	// m_i32Rv이 EVENT_ERROR_SUCCESS 또는 EVENT_ERROR_QUICK_JOIN_2ND_BEST 일때만 유효.
	UINT8			m_ui8ChannelIdx;	// 입장 성공/권유 채널번호.	
	UINT32			m_ui32RoomIdx;		// 입장 성공/권유 방 번호.	

	UINT8			m_ui8ChannelType;	// 채널 타입.ChannelType
	UINT8			m_ui8RoomState;		// 방상태. ROOM_STATE

	// m_i32Rv이 EVENT_ERROR_QUICK_JOIN_2ND_BEST 일때만 유효.
	ROOM_INFO_2ND_BEST 	m_QuickJoin;			// 차선책 추천방 정보.
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ		// 차선책 방 입장 요청. Client -> Game
{
	UINT32			m_ui32ChannelIdx;	// 채널번호.	
	UINT32			m_ui32RoomIdx;		// 방 번호.		
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_QUICKJOIN_ENTER_REC_ROOM_ACK		
//////////////////////////////////////////////////////////////////////////
struct PACKET_QUICKJOIN_ENTER_REC_ROOM_ACK		// 차선책 방 입장 요청.  Game -> Client
{
	INT32			m_i32Rv;			// 결과값. EVENT_ERROR_SUCCESS : 성공,  그외 : 실패
	UINT32			m_ui32ChannelIdx;	// 채널번호.	
	UINT32			m_ui32RoomIdx;		// 방 번호.		
};

#pragma pack(pop)

#endif
