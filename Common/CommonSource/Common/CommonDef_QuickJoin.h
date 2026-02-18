#ifndef __COMMONDEF_QUICKJOIN_H__
#define	__COMMONDEF_QUICKJOIN_H__

#define MAX_QUICK_JOIN_INFO_COUNT		3		// 빠른 입장 설정 정보 갯수.

#define QUICK_JOIN_SEARCH_FAIL_IDX		-1		// 채널,방 번호 검색 실패시.


#pragma pack( push, 1)

// 빠른 입장 설정 정보
struct QUICKJOIN_INFO		
{	
	STAGE_ID	m_ui32StageID = 0;	// 스테이지 옵션.	STAGE_OPTION_TYPE
};


//// 차선책 추천방 정보
struct ROOM_INFO_2ND_BEST			 
{
	QUICKJOIN_INFO 	m_QuickJoin;	// 차선책 추천방 정보.( 스테이지, 스테이지 타입/옵션 )
	ROOM_NOW_SCORE	m_NowScore;		// 차선책 추천방의 배틀 타임 및 스코어 정보.
};



//////////////////////////////////////////////////////////////////////////////////////////////////////
// 서버만 사용.
/////////////////////////////////////////////////////////////////////////////////////////////////////

// 빠른 입장 검색 정보. 
struct QUICKJOIN_SEARCH_INFO			 
{
	STAGE_UID				m_eStageUID = STAGE_UID_NONE;		// IN. 스테이지.		STAGE_UID
	UINT32					m_ui32Rule = 0;			// IN. Rule.			StageID::GetRule();
	INT32					m_i32ChannelIdx = QUICK_JOIN_SEARCH_FAIL_IDX;	// OUT. 채널 번호.
	INT32					m_i32RoomIdx = QUICK_JOIN_SEARCH_FAIL_IDX;		// OUT. 방 번호.
	ROOM_INFO_2ND_BEST		m_RoomInfo2ndBest;	// OUT. 차선책 추천방 정보.
	UINT8					m_ui8ChannelType = CHANNEL_TYPE_NONE;	// OUT. 채널 타입.
	UINT8					m_ui8RoomState = ROOM_STATE_EMPTY;		// 방상태. ROOM_STATE

	void Reset()
	{
		m_eStageUID			= STAGE_UID_NONE;
		m_ui32Rule			= STAGE_MODE_NA;
		m_i32ChannelIdx		= QUICK_JOIN_SEARCH_FAIL_IDX;
		m_i32RoomIdx		= QUICK_JOIN_SEARCH_FAIL_IDX;
		ZeroMemory( &m_RoomInfo2ndBest, sizeof( ROOM_INFO_2ND_BEST ) );
		m_ui8ChannelType	= CHANNEL_TYPE_NONE;
		m_ui8RoomState		= ROOM_STATE_EMPTY;
	}
};

// 빠른 입장 방 입장
struct QUICK_JOIN_ROOM
{
	INT32				m_i32ChannelIdx = 0;	// 채널 번호.
	INT32				m_i32RoomIdx = 0;		// 방 번호.
};

#pragma pack( pop )

#endif // __QUICKJOINDEF_H