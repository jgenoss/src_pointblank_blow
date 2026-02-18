#ifndef __MEDAL_SERVER_DEF_H
#define	__MEDAL_SERVER_DEF_H

///////////////////////////////////////////////////////////////////////////////
// v1.5 Medal Server BEGIN

///////////////////////			Server 전용		///////////////////////////////

//enum MEDAL_MEMORY_TYPE					// 메달 타입. 서버 메모리 기준.
//{
//	MEDAL_MEMORY_TYPE_KILL		= 0,	// Kill미션 관련
//
//	// MEDAL_MC_CHECK_POSITION 랑 맞춥니다.
//	MEDAL_MEMORY_TYPE_RS,				// 뽑기상점.
//	MEDAL_MEMORY_TYPE_SHOP,				// 상점.
//	MEDAL_MEMORY_TYPE_BATTLE,			// 배틀 중
//	MEDAL_MEMORY_TYPE_BATTLEEND,		// 배틀 종료
//	
//	MEDAL_MC_CHECK_POSITION_MAX,
//};


enum MEDAL_SET_GET_TYPE			// 메달 셋트 지급 타입.
{
	MEDAL_SET_GET_TYPE_INSERT		= 0,	// 지급.
	MEDAL_SET_GET_TYPE_REFRESH,				// 갱신.
	
	MEDAL_SET_GET_TYPE_MAX,
};

// 미션 카테고리 체크 플래그.
enum MCC_FLAG		// MISSION_CATEGORY_CHECK_FLAG
{
	MCC_FLAG_TEAM				= 0x0001,	// 팀구분
	MCC_FLAG_TARGET				= 0x0002,	// 제거 대상 
	MCC_FLAG_FIELD				= 0x0004,	// 필드
	MCC_FLAG_LEVEL				= 0x0008,	// 난이도
	MCC_FLAG_ITEM				= 0x0010,	// 아이템
	
	MCC_FLAG_TEAM_ALL			= 0xFFFF	// 전부.
};

#pragma pack( push, 1)

// 메달 시스템 설정정보
struct MEDAL_SYSTEM_INFO	// Control -> Game
{
	BOOL	m_bMedalEnable;					// 메달 시스템 활성화여부.
	UINT32	m_ui32EventMedalSetStartTime;	// 이벤트 메달 셋트 시작 시간. ( 이벤트 활성화 날짜 )
	UINT32	m_ui32EventMedalSetEndTime;		// 이벤트 메달 셋트 종료 시간. ( 이벤트 종료 날짜 )
	UINT16	m_ui16EventMedalSetIdx;			// 이벤트 메달 셋트 고유 Idx.

};

// Medal Set Mqf정보.
struct MQF_MEDAL_SET_INFO
{
	UINT16				m_ui16Idx;					// 메달 세트 고유 인덱스.
	UINT8				m_ui8ActiveMedalCount;		// 활성화 시킬 메달 갯수.
	MQF_MEDAL_INFO		m_aMedalInfo[MAX_MEDAL_COUNT_PER_MEDAL_SET];		// 메달 정보. 단, 메달 세트 내에 메달 레벨은 1만 존재.

	void Reset()
	{
		// 메달 셋트 고유인덱스 초기화.
		m_ui16Idx = INIT_MEDAL_NUMBER; 

		// 메달 고유인덱스 초기화
		for( INT32 i = 0 ; i < MAX_MEDAL_COUNT_PER_MEDAL_SET; i++ )
		{
			m_aMedalInfo[i].m_ui16Idx = INIT_MEDAL_NUMBER;
		}
	}

};

// 전체 일일 Medal Set 정보.
struct MQF_ALL_DAILY_MEDAL_SET_INFO
{
	UINT16						m_ui16ActiveCount;	// 실제 로드된 일일 메달 셋트 갯수.
	MQF_MEDAL_SET_INFO			m_aDailyMedalSetInfo[MAX_LOAD_DAILY_MEDAL_SET_COUNT];		 
};


// 전체 이벤트 Medal Set정보.
struct MQF_ALL_EVENT_MEDAL_SET_INFO
{
	UINT16						m_ui16ActiveCount;	// 실제 로드된 이벤트 메달 셋트 갯수.
	MQF_MEDAL_SET_INFO			m_aEventMedalSetInfo[MAX_LOAD_EVENT_MEDAL_SET_COUNT];	
};

// 전체 일반 메달 MQF 정보.
struct MQF_ALL_MEDAL_INFO	
{
	// 메달 미션 정보. 
	MQF_MEDAL_INFO	m_aMedalInfo[MEDAL_MC_CHECK_POSITION_MAX][MAX_MEDAL_COUNT];

	// 각 타입별 실제 로드된 메달 갯수.
	UINT16				m_aui16MQFMedalCount[MEDAL_MC_CHECK_POSITION_MAX];
	
	void Reset()
	{
		for(INT32 i = 0 ; i < MEDAL_MC_CHECK_POSITION_MAX; i++ )
		{
			m_aui16MQFMedalCount[i] = 0;
			for(INT32 k = 0 ; k < MAX_MEDAL_COUNT; k++ )
			{
				m_aMedalInfo[i][k].m_ui16Idx = INIT_MEDAL_NUMBER;
			}
		}
	}
};


struct MEDAL_GT		// Game - Trans 서버용 일반 메달 
{
	MEDAL	m_Medal;		// 메달. DB 저장 정보.
	UINT8	m_ui8IsSave;	// DB 세이브 여부. 0:No Save, 1:Save. 
};


// 메달 시스템 관련 모든 정보 - 유저기준. Game서버에서 사용
struct MEDAL_ALL_INFO	 
{
	NOTIFY_MEDAL				m_aNotifyMedal[MAX_NOTIFY_MADAL_COUNT];					// 알리미 등록 메달.	

	UINT16						m_aui16MedalCount[MEDAL_MC_CHECK_POSITION_MAX];				// 일반 메달 갯수
	MEDAL_GT					m_aMedal[MEDAL_MC_CHECK_POSITION_MAX][MAX_MEDAL_COUNT];	// 일반 메달 .
	
	CUR_MEDAL_SET				m_aCurMedalSet[MAX_CUR_MEDAL_SET_COUNT];		// 진행중인 메달 셋트.
};

// 메달 정보 DB 기준. Trans서버에서 사용
struct MEDAL_ALL_INFO_DB
{
	UINT16						m_ui16RecvMedalCount;					// Game서버로 부터 받은 일반 메달 갯수.

	NOTIFY_MEDAL				m_aNotifyMedal[MAX_NOTIFY_MADAL_COUNT];	// 알리미 등록 메달.	

	UINT16						m_ui16MedalCount;						// 메달 갯수.
	MEDAL_GT					m_aMedal[MAX_MEDAL_COUNT];				// 메달

	UINT16						m_ui16CurMedalSetCount;					// 진행중인 메달 셋트 갯수.
	CUR_MEDAL_SET				m_aCurMedalSet[MAX_CUR_MEDAL_SET_COUNT];	// 진행중인 메달 셋트.
};


// 메달 미션 완료 체크용 정보
struct MMC_INFO
{
	MEDAL_MC_ACTION_ID	m_eActionID;		// 액션ID.
	MQF_MEDAL_INFO*	m_pMQFMedalInfo;	// MQF 메달 정보
	UINT32				m_ui32UseItemID;	// 사용(또는 획득, 적군이 사용)한 아이템 ID
	UINT8				m_ui8BuyType;		// 구매 유형. 1 : 포인트, 2 : 캐쉬
};

struct MEDAL_REWARD_DETAIL_INFO
{
	INT32	m_i32Rv;			// 결과.
	UINT8	m_ui8MedalType;		// MEDAL_TYPE. 메달 타입.
	UINT16	m_ui16MedalIdx;		// 메달 고유인덱스.

	// 메달 셋트 일떄만.
	UINT8	m_ui8MedalSetType;	// MEDAL_SET_TYPE.
	UINT16	m_ui16MedalSetIdx;	// 메달 셋트 고유 인덱스

	UINT8	m_ui8Level;			// MEDAL_LEVEL. 일반 메달은 메달 레벨(1~4). 셋트 메달은 1 고정.( 의미없음)

	T_ItemID	m_TItemID;		// 지급 ItemID
	T_ItemArg	m_TItemArg;		// 지급 ItemArg

	UINT32	m_ui32Exp;			// 지급 Exp.
	UINT32	m_ui32Point;		// 지급 Point.
	UINT16	m_ui16MasterMedal;	// 마스터 훈장
};



// 메달 보상 정보
struct MEDAL_REWARD_INFO
{
	UINT8	m_ui8Count;			// 보상 갯수
	MEDAL_REWARD_DETAIL_INFO	m_aDetail[MAX_SHOP_DB_ITEM_COUNT];	// 보상 상세 정보
};




struct MEDAL_SET_FILE_NAME
{
	char m_strFileName[MAX_PATH]; 
};


#pragma pack( pop )
//////// Server 전용 끝. ////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////
#endif // __MEDAL_SERVER_DEF_H