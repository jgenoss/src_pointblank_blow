#if !defined( __QUEST_DEF_H)
#define __QUEST_DEF_H

#include "CommonDef.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

/////////////////////////////////////////////////////////////////
// 
// 서버에서 저장용, 네트워크 전달용으로 사용합니다. 
//
// Quest의 진행 상태를 저장하는데 사용되는 자료형
//
typedef struct ALIGN1 _NewMedalFlag
{
	UINT8				m_Miniatureflag[NEW_MEDAL_FLAG_SIZE]; 
	UINT8				m_Insigniaflag[NEW_MEDAL_FLAG_SIZE]; 
	UINT8				m_Orderflag[NEW_MEDAL_FLAG_SIZE]; 	
}NEW_MEDAL_FLAG;

/////////////////////////////////////////////////////////////////
// 
// QUEST_CTX
//
// Quest의 진행 상태를 저장하는데 사용되는 자료형
//
typedef struct ALIGN1 _tagQuestCtx
{
	QUEST_ID			m_ID;								// 각 Quest 마다 가지는 고유한 번호. 모든 User에 걸쳐 동일한 값을 사용한다. Quest의 유형에 따라 전역적으로 고유한 값을 부여한다.

	// 임의 사용 가능한 Argument 변수.
	// Argument의 용도 및 해석에 대해서는 개별 Quest Class의 유형마다 다를 수 있다.
	// 단 이 값이 0xFF로 설정되어 있다면 RANDOM의 의미로 해석되어야 하며, 
	// Client-Side의 각 Quest Class는 해당 값에 대해서 적절한 새로운 값을 설정해 사용하여야 한다.
	// m_Arg[5] = 0xFF 인경우는 완료된 Quest라는 표시
	//0 - Action, 1 - Stage, 2 - Count, 3 - Acc(누적, 연속), 4 - Team, 5 - 카운트
	UINT8				m_Arg[ MAX_QUEST_ARG];
} QUEST_CTX;

/////////////////////////////////////////////////////////////////
// 
// 서버에서 저장용, 네트워크 전달용으로 사용합니다. 
//
// Quest의 진행 상태를 저장하는데 사용되는 자료형
//
typedef struct ALIGN1 _tagQuestChange
{
	UINT8		isExistCompletedQuest;				//완료된 퀘스트가 있는지의 여부
	UINT8		nArgCount;							//변경된 퀘스트 갯수	
	UINT16		nidxQuest[MAX_QUEST_PER_CARD_NEW];	//완료 또는 변경된 퀘스트의 인덱스
	UINT8		ui8QuestActive[ MAX_QUEST_PER_CARD_NEW ];
} QUEST_CHANGE_INFO;


/////////////////////////////////////////////////////////////////
//
// KILL_INFO
//
// 캐릭터가 죽었을 때, Kill Chara에 대한 구체적인 정보를 전달하기 위한 구조체.
// CQuestBase::OnKill 함수와 CQuestBase::OnDeath 함수를 통해 전달된다.
//
struct ALIGN1 KILL_INFO
{
	// GameContext에서의 Slot Index (0-based). 0~15의 값을 가질 수 있다.
	INT32				m_CharaShotIdx;

	// 최종적으로 맞힌 부위
	HIT_CHARA_PART		m_HitPart;

	// 죽은 유저가 사용중인 무기
	WEAPON_CLASS_TYPE	m_TargetWeaponType;

	// 죽은 유저가 사용중인 무기
	INT32				m_TargetWeapon;

	// Piercing Shot인 경우 TRUE, 그렇지 않다면 FALSE 값을 가진다.
	BOOL				m_bPiercingShot;

	// Respawn 이후부터의 누적 Kill Count.
	INT32				m_KillCount;

	// 현재 무기의 Class Type
	WEAPON_CLASS_TYPE	m_WeaponType;

	INT32				m_Weapon;
} ;

//////////////////////////////////////////////////////////////
//
// DAMAGE_INFO
//
// Desc : 공격을 당했을 때, 공격 내용을 전달하기 위한 구조체. CQuestBase::OnHit 함수를 통해 전달된다.
//
struct ALIGN1 DAMAGE_INFO
{
	// 공격자의 GameContext Slot Index (0-based). 0~15의 값을 가질 수 있다.
	INT32				m_AttackerIdx;

	// 피공격자의 GameContext Slot Index.
	INT32				m_HittedIdx;

	// 최종적으로 맞힌 부위
	HIT_CHARA_PART		m_HitPart;

	// Damge량
	REAL32				m_Damage;

	// 공격자의 무기 Class Type
	WEAPON_CLASS_TYPE	m_WeaponType;

	// 공격자의 무기. m_WeaponType의 값에 따라 다른 의미의 값이 전달된다. KILL_INFO Structure의 설명을 참조하기 바란다.
	INT32				m_Weapon;
} ;




//////////////////////////////////////////////////////////////
//
// STAGE_INFO
//
// Desc :	Stage의 시작 및 종료 시에 구체적인 정보를 전달하기 위한 Structure. 
//			CQuestBase::OnStageStart 함수 또는 CQuestBase::OnStageEnd 함수를 통해 전달된다
//
typedef struct ALIGN1 _tagStageInfo
{
	// Stage의 게임 방식에 대한 정의.
	STAGE_MODE		m_StageType;

	// 개발 Stage ID
	STAGE_ID		m_StageID;

	// 최대 입장 허용인 수
	INT32			m_MaxUser;

	// 난입이 가능한지의 여부를 나타낸다. true라면 난입이 가능하다
	bool			m_bCanIntercept;

	// Round제의 Quest인 경우, 제한 Round 수를 가진다. Round제가 아닌 경우, 0이 설정된다.
	INT32			m_LimitRound;

	// Round제의 Quest인 경우, 1 Round의 제한 시간이, 그렇지 않다면 전체 제한 시간을 초 단위로 나타낸다.
	REAL32			m_LimitTime;

	// Red Team의 전적 결과.
	// Deathmatch인 경우 전체 Kill 수를, Round 제 Quest인 경우 승리 Round 수를 의미한다.
	// CQuestBase::OnStageEnd 함수를 통해 전달될 때만 유효하다.
	INT32			m_RedTeamPoint;

	// Blue Team의 전적 결과.
	// Deathmatch인 경우 전체 Kill 수를 의미하며, Round제 Quest인 경우에는 승리 Round 수를 의미한다.
	// CQuestBase::OnStageEnd 함수를 통해 전달될 때에만 유효하다.
	INT32			m_BlueTeamPoint;

	// Battle 또는 Round 종료시 이긴팀을 표시 
	UINT8			m_WinnerTeam;

} STAGE_INFO;

//////////////////////////////////////////////////////////////
//
// ACTION_INFO
//
// Desc :	주인공 캐릭터가 취한 동작 정보를 전달하기 위한 구조체. CQuestBase::OnAction 함수를 통해 전달된다.
//
typedef struct ALIGN1 _tagActionInfo
{
	// 주인공 캐릭터의 GameContext Slot Index. 0~15의 값.
	INT32			m_CharaSlotIdx;

	// 동작의 구체적인 정의
	ACTION_ID		m_Action;
} ACTION_INFO;

//quest들의 active 인덱스 구조체
typedef struct ALIGN1 _tagActiveIndexInfo
{
	UINT8	_idxUse;		//사용중인 cardset 인덱스
	UINT8	_idxCard;		//사용중인 card인덱스
	UINT16	_idxQuest;		//사용중인 quest들의 인덱스 ( 4bit단위)
} QUEST_ACTIVE_INDEX;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//게임서버와 게임툴에서 사용합니다. 혹시 중복된 값들이 있으면 김성백에게 알려주세요. 
#define	QUEST_COLUM_NONE			0
#define	QUEST_COLUM_ACTION			1
#define	QUEST_COLUM_STAGE			2
#define	QUEST_COLUM_COUNT			3
#define	QUEST_COLUM_ACC				4
#define	QUEST_COLUM_TEAM			5
#define	QUEST_COLUM_SAVECOUNT		6
#define	QUEST_COLUM_WEAPON_CLASS	7
#define	QUEST_COLUM_WEAPON_IDX		8

extern const char * g_StrWeaponClass[WEAPON_CLASS_COUNT];
const char * getWeaponClassPostfix_Do( UINT32 WeaponClassIdx);
const char * getWeaponClassPostfix_Use( UINT32 WeaponClassIdx);

typedef struct _QuestInfo
{
	INT32	_pArg[MAX_QUEST_ARG]; 
}QUEST_INFO; 

#define USE_QUEST_COUNT			4
extern const QUEST_INFO g_pQuestInfoTable[USE_QUEST_COUNT];			//쿼스트가 추가될때마다 추가한다. //0번은 사용하지 않음

//Mission Quest File 임무 파일 헤더 ( *.mqf )
typedef struct _QuestFileHeader
{
	 char	_Signature[2];
	INT32	_Version;
	UINT32	_Reserved1; 
	UINT32	_Reserved2; 
	UINT32	_Reserved3; 
	UINT32	_Reserved4; 
}QUEST_FILE_HEADER;

INT32	QuestFileLoad(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO * pGetItem );		//File Load 구버전
BOOL	QuestFileSave(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO * pGetItem );		//File Save 구버전

INT32	QuestFileLoadEx(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO_NEW2 * pGetItem );		//File Load 신버전 2011.4.7 - 서동권
BOOL	QuestFileSaveEx(const char * strFileName, QUEST_CTX * pCard, QUEST_ITEM_INFO_NEW2 * pGetItem );		//File Save 신버전 2011.4.7 - 서동권

STAGE_ID getQuestStageID(UINT8 QuestStageID);

#endif
