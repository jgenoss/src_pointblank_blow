#ifndef __PROTOCOL_MEDAL_STRUCT_H__
#define __PROTOCOL_MEDAL_STRUCT_H__

#pragma pack( push, 1)

// v1.5 Medal
//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_MEDAL_INFO_REQ		
//////////////////////////////////////////////////////////////////////////
struct PACKET_GET_MEDAL_INFO_REQ		// 모든 메달 정보 요청. Client -> Game.
{
	// 내용없음.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_MEDAL_INFO_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_GET_MEDAL_INFO_ACK		// 모든 메달 정보 응답 헤더. Game -> Client.
{
	INT32				m_i32Rv;			// 결과값.
	UINT16				m_ui16TotalCount;	// 모든 메달 갯수.	최대 MAX_NORMAL_MEDAL_COUNT
	UINT16				m_ui16Count;		// 이 패킷에서 내려줄 메달 갯수.	최대 m_ui16TotalMedalCount
	UINT16				m_ui16Idx;			// 이 패킷에서 시작할 메달 순서번호.

	// Data
	// MEDAL	m_Medal;	 // 메달
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_CUR_MEDAL_SET_INFO_REQ		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_GET_CUR_MEDAL_SET_INFO_REQ	// 진행중인 메달 셋트 정보 요청. Client -> Game.
{
	// 내용없음.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_CUR_MEDAL_SET_INFO_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_GET_CUR_MEDAL_SET_INFO_ACK	// 진행중인 메달 셋트 정보 응답 헤더. Game -> Client.
{
	INT32			m_i32Rv;			// 결과값.
	UINT16			m_ui16TotalCount;	// 모든 진행중인 메달 셋트 갯수.	최대 MAX_CUR_MEDAL_SET_COUNT
	UINT16			m_ui16Count;		// 이 패킷에서 내려줄 진행중인 메달 셋트 갯수.	최대 m_ui16TotalMedalCount
	UINT16			m_ui16Idx;			// 이 패킷에서 시작할 진행중인 메달 셋트 순서번호.

	// Data
	// CUR_MEDAL_SET	m_aCurMedalSet;	// 진행중인 메달 셋트.
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_COM_MEDAL_SET_INFO_REQ		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_GET_COM_MEDAL_SET_INFO_REQ	// 완료된 메달 셋트 정보 요청. Client -> Game.
{
	// 내용없음.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_COM_MEDAL_SET_INFO_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_GET_COM_MEDAL_SET_INFO_ACK	// 완료된 메달 셋트 정보 응답 헤더. Game -> Client.
{
	INT32			m_i32Rv;			// 결과값.
	UINT16			m_ui16TotalCount;	// 모든 완료된 메달 셋트 갯수.	최대 MAX_COM_MEDAL_SET_COUNT
	UINT16			m_ui16Count;		// 이 패킷에서 내려줄 완료된 메달 셋트 갯수.	최대 m_ui16TotalMedalCount
	UINT16			m_ui16Idx;			// 이 패킷에서 시작할 완료된 메달 셋트 순서번호.

	// Data
	// COM_MEDAL_SET	m_aComMedalSet;	// 완료된 메달 셋트.
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MISSION_COMPLETE_ACK		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_MISSION_COMPLETE_ACK	// 메달 미션 완료(미션카운트증가) 응답 헤더. Game -> Client. v1.0 : PROTOCOL_BASE_QUEST_CHANGE_ACK
{
	UINT16			m_ui16CompleteMedalCount;	// 미션 달성한 메달 갯수.

	// Data
	//	MEDAL_COMPLETE m_MedalComplete; // m_ui16CompleteMedalCount 개.
};



//////////////////////////////////////////////////////////////////////////
// PROTOCOL_COMPLETE_ACK		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_COMPLETE_ACK		// 메달 완료(1개 레벨) ACK. Game -> Client. v1.0 : PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK
{
	UINT8			m_ui8MedalType;		// MEDAL_TYPE. 메달 타입. ( 메달용인지, 메달 세트용 인지 )
	UINT16			m_ui16MedalIdx;		// 완료된 메달 고유 인덱스. 
	MEDAL_LEVEL		m_eMedalLevel;		// 완료한 레벨.
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SET_COMPLETE_ACK		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_SET_COMPLETE_ACK	// 메달 셋트 완료(세트내의 모든 메달완료) ACK. Game -> Client
{
	UINT8			m_ui8MedalType;		// MEDAL_SET_TYPE. 메달 셋트 타입.  
	UINT16			m_ui16MedalSetIdx;	// 완료된 메달 셋트 고유 인덱스. 
};




//////////////////////////////////////////////////////////////////////////
// PROTOCOL_GET_MEDAL_REWARD_REQ		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_GET_MEDAL_REWARD_REQ	// 메달 완료(n개)에 대한 보상 요청. Client -> Game.
{
	UINT8			m_ui8Count;			// 요청할 메달 갯수. 최대 MAX_SHOP_DB_ITEM_COUNT 개.

	// m_ui8Count 만큼 MEDAL_REWARD 사이즈로 파싱합니다.
};

//////////////////////////////////////////////////////////////////////////
// PACKET_H_GET_MEDAL_REWARD_ACK		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_GET_MEDAL_REWARD_ACK	// 메달 완료에 대한 보상 응답. Game -> Client. 추가로 .메달 갱신 여부 및 갱신된 메달 정보.
{
	bool			m_bLastPacket;		// 마지막 패킷인지.
	UINT8			m_ui8Count;			// 요청할 메달 갯수. 최대 MAX_SHOP_DB_ITEM_COUNT 개. 
	UINT32			m_ui32Exp;			// 갱신된 EXP
	UINT32			m_ui32Point;		// 갱신된 Point
	UINT16			m_ui16MasterMedal;	// 갱신된 마스터 훈장.

	// m_ui8Count 만큼 PACKET_D_GET_MEDAL_REWARD 사이즈로 파싱합니다.
};

struct PACKET_D_GET_MEDAL_REWARD	// 메달 완료(1개 레벨)에 대한 보상 응답. Game -> Client. 추가로 .메달 갱신 여부 및 갱신된 메달 정보.
{
	INT32			m_i32Rv;			// 처리 결과.
	MEDAL_REWARD	m_aMedalReward;		// 보상 메달 정보
	ST_ITEM			m_aItem;			// 보상 아이템 정보
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SET_NOTIFY_MEDAL_REQ		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_SET_NOTIFY_MEDAL_REQ	// 알리미 메달 설정 요청. Client -> Game. ( 메달 페이지 나갈때만 던집니다. )
{
	NOTIFY_MEDAL		m_aNotifyMedal[MAX_NOTIFY_MADAL_COUNT];		// 알리미 등록 메달.	
};
//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SET_NOTIFY_MEDAL_ACK
//////////////////////////////////////////////////////////////////////////
struct PACKET_SET_NOTIFY_MEDAL_ACK	// 알리미 메달 설정 응답. Game -> Client
{
	INT32				m_i32Rv;			// 처리 결과.
	NOTIFY_MEDAL		m_aNotifyMedal[MAX_NOTIFY_MADAL_COUNT];		// 알리미 등록 메달.	
};




//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SET_REFRESH_MEDAL_SET_ACK		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_SET_REFRESH_MEDAL_SET_ACK	// 메달 셋트 갱신 여부. Game -> Client. 로그인시, 룸/클랜/상점/뽑기상점 입장시.
{
	CUR_MEDAL_SET	m_aCurMedalSet[MAX_CUR_MEDAL_SET_COUNT];	// 진행중인 메달 셋트 정보 전부.
};


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MEDAL_COMMAND_REQ		 
//////////////////////////////////////////////////////////////////////////
struct PACKET_MEDAL_COMMAND_REQ	// 메달 Command. Debug 모드에서만 동작합니다. ACK 필요없음.
{
	MEDAL_COMMAND	m_MedalCommand;	 
};
//////////////////////////////////////////////////////////////////////////
// PROTOCOL_MEDAL_COMMAND_ACK 
//////////////////////////////////////////////////////////////////////////
struct PACKET_MEDAL_COMMAND_ACK	// 메달 Command. Debug 모드에서만 동작합니다. ACK 필요없음.
{
	INT32			m_i32Rv;		// 성공시 EVENT_ERROR_SUCCESS
	MEDAL_COMMAND	m_MedalCommand;
};




#pragma pack(pop)

#endif
