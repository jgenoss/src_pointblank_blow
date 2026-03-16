#ifndef __MEDALDEF_H
#define	__MEDALDEF_H

#define NORMAL_MEDAL_PATH_NONE			"Quest/Normal/NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_MATCH			"Quest/Normal/NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_KOREA			"Quest/Normal/KR_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_THAILAND		"Quest/Normal/TH_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_INDONESIA		"Quest/Normal/ID_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_RUSSIA		"Quest/Normal/RU_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_TURKEY		"Quest/Normal/TR_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_CHINA			"Quest/Normal/CH_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_TAIWAN		"Quest/Normal/TW_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_JAPAN			"Quest/Normal/JP_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_BRAZIL		"Quest/Normal/BR_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_LATIN_AMERICA	"Quest/Normal/LAM_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_NORTHAMERICA	"Quest/Normal/US_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_ITALY			"Quest/Normal/IT_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_MIDDLE_EAST	"Quest/Normal/MENA_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_PHILIPPINES	"Quest/Normal/PH_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_SINGMAL		"Quest/Normal/SAM_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_VIETNAM		"Quest/Normal/VT_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_KOREA_DAUM	"Quest/Normal/DA_NormalMedal.mqfex"
#define NORMAL_MEDAL_PATH_KOREA_PICAON	"Quest/Normal/PI_NormalMedal.mqfex"

extern const char		g_strNomalMedalPath[LOCALE_CODE_MAX][MAX_PATH];

///////////////////////////////////////////////////////////////////////////////
// v1.5 Medal BEGIN
#define		COMMAND_MEDAL_COUNT			"/MEDALC"	// 메달 카운트 증가 커맨드. ex : /MEDALC MEDAL_TYPE MedalIdx MedalCount
#define		COMMAND_MEDAL_SET_CHANGE	"/MEDALS"	// 셋트 메달 교체.			ex : /MEDALS MEDAL_SET_TYPE SetMedalIdx

enum MEDAL_COMMAND_TYPE
{
	MEDAL_COMMAND_TYPE_UNKNOWN = 0,	
	MEDAL_COMMAND_TYPE_COUNT,				// /MEDALC
	MEDAL_COMMAND_TYPE_CHANGE_MEDALSET,		// /MEDALS
	
	MEDAL_COMMAND_TYPE_MAX,
};

// 일반 메달 카운트일때 Argument 순번
enum MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING
{
	MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_COMMAND	= 0,	
	MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_TYPE,	
	MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_IDX,				 
	MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MEDAL_COUNT,				 

	MEDAL_COMMAND_TYPE_COUNT_ARG_ORDERING_MAX,
};


#define MAX_MEDAL_COUNT						100		// 일반 메달 최대 갯수
#define MAX_LOAD_DAILY_MEDAL_SET_COUNT		100		// 게임 서버에서 로드하는 일일메달 세트 최대 갯수
#define MAX_LOAD_EVENT_MEDAL_SET_COUNT		200		// 게임 서버에서 로드하는 이벤트메달 세트 최대 갯수
#define MAX_CUR_DAILY_MEDAL_SET_COUNT		1		// 진행중인 일일 메달 세트 최대 갯수. 
#define MAX_CUR_EVENT_MEDAL_SET_COUNT		1		// 진행중인 이벤트 메달 세트 최대 갯수. 
#define MAX_CUR_MEDAL_SET_COUNT				2		// 진행중인 메달 셋트 갯수. 배열 인덱스는 일일0, 이벤트1

#define MAX_COM_MEDAL_SET_COUNT				50		// 완료된 메달 세트 최대 갯수. 
#define MAX_MEDAL_COUNT_PER_MEDAL_SET		6		// 메달 세트 1개당 최대 메달 수 
#define MAX_MADAL_REWARD_COUNT_PER_LEVEL	3		// 레벨별 메달 보상 최대갯수.
#define MAX_NOTIFY_MADAL_COUNT				4		// 알리미 등록 메달 갯수.

#define MAX_STRING_MEDAL_NAME_KEY_COUNT			128
#define MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT	128

#define	INIT_MEDAL_NUMBER					0xFFFF	// 메달 고유인덱스 최대값 = 설정 안된값.

enum MEDAL_TYPE			// 메달 타입.
{
	MEDAL_TYPE_NORMAL		= 0,
	MEDAL_TYPE_SET,	
	
	MEDAL_TYPE_MAX,
};

enum MEDAL_SET_TYPE		// 메달 세트 타입.
{
	MEDAL_SET_TYPE_NORMAL		= 0,
	MEDAL_SET_TYPE_EVENT,
	MEDAL_SET_TYPE_DAILY,			
	MEDAL_SET_TYPE_CLAN,		

	MEDAL_SET_TYPE_MAX,
};


enum MEDAL_SET_STATE		// 메달 세트 상태
{
	MEDAL_SET_STATE_CURRENT		= 0,	// 진행중인 메달 셋트
	MEDAL_SET_STATE_COMPLETE,			// 완료된 메달 셋트
	
	MEDAL_SET_STATE_MAX,
};

enum MEDAL_LEVEL		// 메달 레벨
{
	MEDAL_LEVEL_NONE	= 0,
	MEDAL_LEVEL_1,	
	MEDAL_LEVEL_MAX_SET = MEDAL_LEVEL_1,
	MEDAL_LEVEL_2,		
	MEDAL_LEVEL_3,		
	MEDAL_LEVEL_4,		
 	MEDAL_LEVEL_MAX = MEDAL_LEVEL_4,
};



// Mission Categorie
// 행동 ID		// v1.0 ACTION_ID
enum MEDAL_MC_ACTION_ID
{
	MEDAL_MC_ACTION_ID_NA			= 0,				// 정의되지 않음.
	
	// MEDAL_MC_CHECK_POSITION_BATTLE	
	MEDAL_MC_ACTION_ID_C4_PLANT,						// C4 설치하기
	MEDAL_MC_ACTION_ID_C4_DEFUSE,						// C4 해체하기
	MEDAL_MC_ACTION_ID_STAGE_ENTER,						// 입장
	MEDAL_MC_ACTION_ID_STAGE_INTERCEPT,					// 난입
	MEDAL_MC_ACTION_ID_KILL,							// Kill
	MEDAL_MC_ACTION_ID_DEATH,							// Death
	MEDAL_MC_ACTION_ID_SUICIDE,							// 자살. 사용하지 않음.
	MEDAL_MC_ACTION_ID_HEADSHOT,						// Head shot 달성
	MEDAL_MC_ACTION_ID_DOUBLE_KILL,						// Double Kill 달성
	MEDAL_MC_ACTION_ID_PIERCING,						// Piercing Kill 달성
	MEDAL_MC_ACTION_ID_TRIPLE_KILL,						// Triple Kill 달성
	MEDAL_MC_ACTION_ID_MASS_KILL,						// Mass Kill
	MEDAL_MC_ACTION_ID_DESTROY,							// 목표물 파괴
	MEDAL_MC_ACTION_ID_USING_WEAPON_ENEMY_KILL,			// 사용하지 않음. 특정 무기 사용중인 적군을 킬 // v1.0 : ACTION_ID_KILL_WEAPONCLASS	
	MEDAL_MC_ACTION_ID_USE_WEAPON_KILL,					// 특정 무기 사용해서 킬 // v1.0 : ACTION_ID_USE_WEAPON
	MEDAL_MC_ACTION_ID_CHAINKILLER,						// 체인킬
	MEDAL_MC_ACTION_ID_CHAINSLUGGER,					// 체인 슬러거
	MEDAL_MC_ACTION_ID_CHAINSTOPPER,					// 체인 스타퍼
	MEDAL_MC_ACTION_ID_CHAINHEADSHOT,					// 체인 헤드샷
	MEDAL_MC_ACTION_ID_PROTECT,							// 오브젝트 보호
	MEDAL_MC_ACTION_ID_DEATHBLOW,						// 데스블로우
	MEDAL_MC_ACTION_ID_TOUCHDOWN,						// 터치다운
	MEDAL_MC_ACTION_ID_TOUCHDOWN_ACE_ATTACKER,			// 에이스 어태커
	MEDAL_MC_ACTION_ID_TOUCHDOWN_HATTRICK,				// 해트트릭
	MEDAL_MC_ACTION_ID_TOUCHDOWN_GAME_MAKER,			// 게임메이커
	MEDAL_MC_ACTION_ID_KILL_ASSIST,						// 킬 어시스트

	MEDAL_MC_ACTION_ID_BATTLE_MAX = MEDAL_MC_ACTION_ID_KILL_ASSIST,		
	

	// v1.5 Medal 에서 추가된 액션.
	// MEDAL_MC_CHECK_POSITION_SHOP
	MEDAL_MC_ACTION_ID_BUY			= 500,				// 구매
	MEDAL_MC_ACTION_ID_BUY_BY_POINT,					// 포인트로 구매
	MEDAL_MC_ACTION_ID_BUY_BY_CASH,						// 캐쉬로 구매

	MEDAL_MC_ACTION_ID_SHOP_MAX = MEDAL_MC_ACTION_ID_BUY_BY_CASH,						//  


	// MEDAL_MC_CHECK_POSITION_RS
	MEDAL_MC_ACTION_ID_RS_BUY		= 600,				// 뽑기상점 당첨
	MEDAL_MC_ACTION_ID_RS_JACKPOT,						// 뽑기상점 잭팟 당첨

	MEDAL_MC_ACTION_ID_RS_MAX = MEDAL_MC_ACTION_ID_RS_JACKPOT,			


	// MEDAL_MC_CHECK_POSITION_BATTLE_END
	MEDAL_MC_ACTION_ID_RESULT_RANK	= 700,				// 최종 결과 순위
	MEDAL_MC_ACTION_ID_DRAW,							// 무승부
	MEDAL_MC_ACTION_ID_WIN,								// 승리
	MEDAL_MC_ACTION_ID_DEFEAT,							// 패배

	MEDAL_MC_ACTION_ID_BATTLE_END_MAX = MEDAL_MC_ACTION_ID_DEFEAT,		


	//MEDAL_MC_ACTION_ID_MAX, //맥스 값 체크 불가능
};

enum MEDAL_MC_CHECK_POSITION	// 미션 카테고리. 체크 포지션. 체크조건 1순위.
{
	MEDAL_MC_CHECK_POSITION_NONE		= 0,
	MEDAL_MC_CHECK_POSITION_START,
	MEDAL_MC_CHECK_POSITION_RS = MEDAL_MC_CHECK_POSITION_START,
	MEDAL_MC_CHECK_POSITION_SHOP,
	MEDAL_MC_CHECK_POSITION_BATTLE,
	MEDAL_MC_CHECK_POSITION_BATTLE_END,
	MEDAL_MC_CHECK_POSITION_MAX,
};



enum MEDAL_MC_TEAM	// 미션 카테고리. 팀 구분.
{
	MEDAL_MC_TEAM_NONE	= 0,	// 팀 상관 없음.
	MEDAL_MC_TEAM_RED	,		// 레드 
	MEDAL_MC_TEAM_BLUE	,		// 블루
	MEDAL_MC_TEAM_HUMAN	,		// 인간 
	MEDAL_MC_TEAM_DINO	,		// 공룡 

	MEDAL_MC_TEAM_MAX
};

enum MEDAL_MCF_FIELD		// 미션 카테고리 플래그. 전장.
{
	MEDAL_MCF_FIELD_NONE = 0,		// 없음.
	MEDAL_MCF_FIELD_STAGE	,		// 스테이지 모드	enum STAGE_MODE
	MEDAL_MCF_FIELD_WEAPON	,		// 무기제한			enum WEAPON_RESTRICTION
	MEDAL_MCF_FIELD_MAP		,		// 맵

	MEDAL_MCF_FIELD_MAX
};

enum MEDAL_MCF_ITEM		// 미션 카테고리 플래그. 아이템.
{
	MEDAL_MCF_ITEM_NONE = 0,		// 없음.
	MEDAL_MCF_ITEM_TYPE	,			// 아이템 타입
	MEDAL_MCF_WEAPON_TYPE,			// 무기 타입
	MEDAL_MCF_ITEM_ID,				// 아이템 ID

	MEDAL_MCF_ITEM_MAX
};

#define MAX_MADAL_REWARD_COUNT  MAX_MADAL_REWARD_COUNT_PER_LEVEL*MEDAL_LEVEL_MAX						// 메달(1개) 보상 최대 갯수.
#define MAX_MADAL_SET_REWARD_COUNT  MAX_MADAL_REWARD_COUNT_PER_LEVEL*MAX_MEDAL_COUNT_PER_MEDAL_SET		// 메달세트 보상 최대 갯수.

#pragma pack( push, 1)

struct MEDAL		// 메달
{
	UINT16	m_ui16Idx;	// 메달 고유 Idx.
	UINT16	m_ui16Count;	// 미션 진행 횟수. 

	UINT8	m_ui8GetReward;	// 보상 아이템 지급여부. ( 1 1 1 1 레벨별로 비트연산. 가장 우측 비트부터 1레벨) 
};
 
struct CUR_MEDAL_SET	// 진행중인 메달 세트 ( 일일, 이벤트 )
{
	// 주의 m_ui8Type이 MEDAL_SET_TYPE_NORMAL 이거나 m_ui16Idx 가 INIT_MEDAL_NUMBER 이면 없는 메달 셋트 입니다.

	UINT8	m_ui8Type;			// MEDAL_SET_TYPE. 메달 세트 타입.
	UINT16	m_ui16Idx;			// 메달 세트 고유 인덱스. 타입에 따라 독립적으로 간다.
	UINT32	m_ui32RecvDate;		// 메달 지급 받은 날짜. ( 보상 지급이 아님 )
	UINT16	m_ui16Count[MAX_MEDAL_COUNT_PER_MEDAL_SET]; // 메달별 미션 진행횟수

	// 셋트용 메달은 Lv1씩만 존재합니다.
	UINT8	m_ui8GetReward;	// 보상 아이템 지급여부. ( 1 1 1 1 1 1 메달별로 비트연산. 가장 우측 비트부터 1번째 메달) 
};

struct COM_MEDAL_SET	// 완료된 메달 세트 
{
	UINT8			m_ui8Type;			// MEDAL_SET_TYPE. 메달 세트 타입.
	UINT16			m_ui16Idx;			// 메달 세트 고유 인덱스. 타입에 따라 독립적으로 간다.
	UINT32			m_ui32RecvDate;		// 메달 지급 받은 날짜. ( 보상 지급이 아님 )
	//UINT32			m_ui32CompleteDate;	// 미션 완료 날짜. DB에만 둡니다. 서버에 둘필요는 없음.
	UINT16			m_ui16Count[MAX_MEDAL_COUNT_PER_MEDAL_SET]; // 메달별 미션 진행횟수

	// 셋트용 메달은 Lv1씩만 존재합니다.
	UINT8			m_ui8GetReward;	// 보상 아이템 지급여부. ( 1 1 1 1 1 1 메달별로 비트연산. 가장 우측 비트부터 1번째 메달)  
};

struct MEDAL_STATE	 // 메달 진행 상태 
{
	UINT8			m_ui8MedalType;		// MEDAL_TYPE. 메달 타입. 
	UINT16			m_ui16MedalIdx;		// 메달 고유 인덱스.
	UINT16			m_ui16Count;		// 미션 달성 횟수.
};

struct MEDAL_REWARD		//  메달 보상 정보
{
	UINT8			m_ui8MedalType;		// MEDAL_TYPE. 메달 타입.
	UINT16			m_ui16MedalIdx;		// 메달 고유 인덱스. 
	UINT8			m_ui8MedalLevel;	// 일반 메달은 메달 레벨(1~4). 셋트 메달은 지급받을 메달 순번(1~6).
};




//////////////////////////////////////////////////////////////////
// MQF 파일


// Medal 보상정보.
struct MQF_REWARD_INFO_OLD
{
	// 보상
	UINT32				m_ui32RewardExp;			// 지급 Exp.
	UINT32				m_ui32RewardPoint;			// 지급 Point.
	UINT32				m_ui32RewardItemID;			// 지급 ItemID.
	UINT32				m_ui32RewardItemCount;		// 지급 ItemCount.
	UINT32				m_ui32RewardItemPeriod;		// 지급 ItemPeriod.
};


// Medal 보상정보.
struct MQF_REWARD_INFO
{
	// 보상
	UINT32				m_ui32RewardExp;			// 지급 Exp.
	UINT32				m_ui32RewardPoint;			// 지급 Point.
	UINT32				m_ui32RewardItemID;			// 지급 ItemID.
	UINT32				m_ui32RewardItemCount;		// 지급 ItemCount.
	UINT32				m_ui32RewardItemPeriod;		// 지급 ItemPeriod.
	UINT32				m_ui32RewardMaster;			// 지급 마스터훈장
	UINT32				pad[32];
};


// Medal MQF 정보 - 메달 툴에서 Save되는 형태.
struct MQF_MEDAL_INFO_OLD
{
	// 생성자
	MQF_MEDAL_INFO_OLD();

	MEDAL_TYPE					m_eMedalType;		// 메달 타입.
	UINT16						m_ui16Idx;			// 메달 고유 인덱스.

	// 미션
	MEDAL_MC_CHECK_POSITION		m_eCheckPosition;	// 미션 체크 포지션. 체크조건 1순위	
	MEDAL_MC_ACTION_ID			m_eActionID;		// 액션 아이디. 체크조건 2순위. 
	MEDAL_MC_TEAM				m_eTeam;			// 팀 구분.
	UINT8						m_ui8Temp;			// 사용안함. 
	RANK_COUNT					m_eTargetRankBegin;	// 제거 대상의 랭크 범위 시작.
	RANK_COUNT					m_eTargetRankEnd;	// 제거 대상의 랭크 범위 끝. 

	// 이 셋중 하나만 값이 있어야 정상입니다.
	MEDAL_MCF_FIELD				m_eField;			// 필드 플래그.( 아래 항목중 어떤걸 사용할지 )
	STAGE_MODE					m_eStage;			// 스테이지 타입.	
	STAGE_UID					m_eMap;				// 맵.

	// 이 셋중 하나만 값이 있어야 정상입니다.
	MEDAL_MCF_ITEM				m_eItem;			// 아이템 플래그.( 아래 항목중 어떤걸 사용할지 )
	ITEM_TYPE					m_eUseItemType;		// 사용(또는 획득) ItemType. ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.
	WEAPON_CLASS_TYPE			m_eWeaponType;		// 사용(또는 획득) WeaponType. ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.
	UINT32						m_ui32UseItemID;	// 사용(또는 획득) ItemID.   ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.

	// 스테이지 난이도.  STAGE_MASK_TYPE이 STAGE_MASK_TYPE_CHALLENGE일때 1~10.
	UINT8						m_ui8StageLevel;	
	// 레벨별 달성횟수.
	UINT16						m_ui16Count[MEDAL_LEVEL_MAX];
	// 레벨별 보상
	MQF_REWARD_INFO_OLD			m_eRewardInfo[MEDAL_LEVEL_MAX];

	UINT16						m_ui16ImageIdx;		// 메달 이미지 인덱스	
	char						m_szNameKey[ MAX_STRING_MEDAL_NAME_KEY_COUNT ];				// 메달 명 스트링 키 (클라이언트 GAME_STRING 용)
	char						m_szDescription[ MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT ];	// 메달 설명 스트링 키 (클라이언트 GAME_STRING 용)
};


// Medal MQF 정보 - 메달 툴에서 Save되는 형태.
struct MQF_MEDAL_INFO
{
	// 생성자
	MQF_MEDAL_INFO();
	void SetOldData(MQF_MEDAL_INFO_OLD* old);

	MEDAL_TYPE			m_eMedalType;		// 메달 타입.
	UINT16				m_ui16Idx;			// 메달 고유 인덱스.
	
	// 미션
	MEDAL_MC_CHECK_POSITION		m_eCheckPosition;		// 미션 체크 포지션. 체크조건 1순위	
	MEDAL_MC_ACTION_ID			m_eActionID;			// 액션 아이디. 체크조건 2순위. 
	MEDAL_MC_TEAM				m_eTeam;				// 팀 구분.
	UINT8						m_ui8Temp;				// 사용안함.
	RANK_COUNT					m_eTargetRankBegin;		// 제거 대상의 랭크 범위 시작.
	RANK_COUNT					m_eTargetRankEnd;		// 제거 대상의 랭크 범위 끝. 

	// 이 셋중 하나만 값이 있어야 정상입니다.
	MEDAL_MCF_FIELD				m_eField;			// 필드 플래그.( 아래 항목중 어떤걸 사용할지 )
	STAGE_MODE					m_eStage;			// 스테이지 타입.
	WEAPON_RESTRICTION			m_eWeapon;			// 무기제한.
	STAGE_UID					m_eMap;				// 맵.

	// 이 셋중 하나만 값이 있어야 정상입니다.
	MEDAL_MCF_ITEM				m_eItem;			// 아이템 플래그.( 아래 항목중 어떤걸 사용할지 )
	ITEM_TYPE					m_eUseItemType;		// 사용(또는 획득) ItemType. ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.
	WEAPON_CLASS_TYPE			m_eWeaponType;		// 사용(또는 획득) WeaponType. ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.
	UINT32						m_ui32UseItemID;	// 사용(또는 획득) ItemID.   ACTION_ID가 ACTION_ID_USE_WEAPON이거나 ACTION_ID_KILL_WEAPONCLASS 이면 사용. ACTION_ID가 구매 및 당첨 관련이면 획득.

	// 스테이지 난이도.  STAGE_MODE 이 STAGE_MODE_DOMINATION 일때 1~5. STAGE_MASK_TYPE이 STAGE_MASK_TYPE_TRAINING일때 1~10.
	UINT8				m_ui8StageLevel;	
	// 레벨별 달성횟수.
	UINT16				m_ui16Count[MEDAL_LEVEL_MAX];
	// 레벨별 보상
	MQF_REWARD_INFO		m_eRewardInfo[MEDAL_LEVEL_MAX];


	UINT16				m_ui16ImageIdx;		// 메달 이미지 인덱스	
	char				m_szNameKey[ MAX_STRING_MEDAL_NAME_KEY_COUNT ];				// 메달 명 스트링 키 (클라이언트 GAME_STRING 용)
	char				m_szDescription[ MAX_STRING_MEDAL_DESCRIPTION_KEY_COUNT ];	// 메달 설명 스트링 키 (클라이언트 GAME_STRING 용)

	UINT32			pad[32];
};


// 일일 Medal Set정보.
struct MQF_DAILY_MEDAL_SET_INFO	
{
	UINT16				m_ui16Idx;					// 메달 세트 고유 인덱스.
	UINT8				m_ui8ActiveMedalCount;		// 활성화 시킬 메달 갯수.
	MQF_MEDAL_INFO		m_MedalInfo[MAX_MEDAL_COUNT_PER_MEDAL_SET];		// 메달 정보. 단, 메달 세트 내에 메달 레벨은 1만 존재.
};



// MQF 파일
//////////////////////////////////////////////////////////////////

// 알리미 등록 메달.
struct NOTIFY_MEDAL
{
	UINT8				m_ui8MedalType;		// MEDAL_TYPE. 메달 타입.
	UINT16				m_ui16Idx;			// 메달 고유 인덱스
	UINT16				m_ui16Count;		// 진행 횟수. 
};

// 메달 시스템 파일 헤더 (*.mqfex)
struct MQF_MEDAL_FILEHEADER
{
	MQF_MEDAL_FILEHEADER() 
		: m_eMedalSetType	( MEDAL_SET_TYPE_NORMAL )
		, m_ui32MedalCount	( 0 )
		, m_ui32Idx			( (UINT32) -1 )
		, m_ui32Version		( 0 )
		, m_ui32Reserved1	( 0 )
		, m_ui32Reserved2	( 0 )
	{
		m_szSignature[0] = 0;
		m_szSignature[1] = 0;
		m_szSignature[2] = 0;
	}

	char m_szSignature[3];

	MEDAL_SET_TYPE	m_eMedalSetType;
	UINT32			m_ui32MedalCount;
	UINT32			m_ui32Idx;
	UINT32			m_ui32Version;
	
	UINT32			m_ui32Reserved1;
	UINT32			m_ui32Reserved2;
};

// 메달 치트키 
struct MEDAL_COMMAND
{
	UINT8	m_ui8CommandType;	// MEDAL_COMMAND_TYPE
	UINT8	m_ui8MedalType;		// MEDAL_TYPE. 메달 타입.
	UINT16	m_ui16Idx;			// 메달 고유 인덱스.
	UINT16	m_ui16Count;		// 진행 횟수. m_ui8CommandType 이 MEDAL_COMMAND_TYPE_COUNT 일때.
};

#pragma pack( pop )






class CMedalSystemFile
{
public:
	CMedalSystemFile();
	~CMedalSystemFile();

public:
	enum
	{
		MEDAL_FILE_VERSION_1		= 0x00000001,
		MEDAL_FILE_VERSION_CURRENT	= 0x00000002,
	};

public:
	MQF_MEDAL_INFO*		GetMedalInfo( int idx )
	{
		if( idx < 0 || static_cast<int>(m_header.m_ui32MedalCount) <= idx )
		{
			return nullptr;
		}

		return &m_pInfo[idx];
	}

	MEDAL_SET_TYPE		GetMedalSetType() const		{ return m_header.m_eMedalSetType; }
	unsigned int		GetMedalInfoCount() const	{ return m_header.m_ui32MedalCount; }
	unsigned int		GetMedalUniqueIdx() const	{ return m_header.m_ui32Idx; }
	unsigned int		GetVersion() const			{ return m_header.m_ui32Version; }

public:
	void	SetAutoMemoryDelete( bool bAuto = true )
	{
		m_bAutoMeoryDelete = bAuto;
	}

	void	SetMedalSetType( MEDAL_SET_TYPE eType );
	void	SetMedalUiqueIdx( unsigned int idx );
	void	SetMedalInfo( MQF_MEDAL_INFO* pInfo, unsigned int uCount );

public:
	bool	LoadMQF( const char* psz );
	bool	SaveMQF( const char* psz );
	void	Destroy();

	bool	LoadMQFinPack( const char* psz );
	bool	SaveMQFinPack( const char* psz );

private:
	MQF_MEDAL_FILEHEADER	m_header;
	MQF_MEDAL_INFO*			m_pInfo;

	HANDLE		m_hf;

	bool		m_bAutoMeoryDelete;
};

#endif // __MEDALDEF_H