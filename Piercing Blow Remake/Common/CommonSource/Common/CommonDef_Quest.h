#ifndef __COMMONDEF_QUEST_H__
#define __COMMONDEF_QUEST_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
// New 버전입니다. Define 새로 갑니다. 이유는.. 추후에 클라이언트 까지 바뀌면 위에꺼 삭제.
#define		MAX_QUEST_PER_CARD_NEW				4		//4 한개의 카드가 가질 수 있는 퀘스트
#define		MAX_CARD_PER_CARDSET				10		//각각 P,O,I,N,T,B,L,A,N2,K 총 10개의 카드가 한개의 카드셋안에 들어있다.
#define		MAX_CARDSET_PER_USER				4		//4가지 종류의 카드를 저장한다.
#define		TUTORIAL_CARDSET_SLOT				0
#define		EVENT_CARDSET_SLOT					(MAX_CARDSET_PER_USER - 1)
#define		MAX_QUEST_PER_CARDSET_NEW			(MAX_QUEST_PER_CARD_NEW * MAX_CARD_PER_CARDSET)							//4 * 10	총 40개입니다.
#define		MAX_QUEST_ALL_CARD					(MAX_CARD_PER_CARDSET * MAX_CARDSET_PER_USER)							//10 * 4 
#define		MAX_QUEST_ALL_USE_NEW				(MAX_QUEST_PER_CARD_NEW * MAX_CARD_PER_CARDSET * MAX_CARDSET_PER_USER)	//4 * 10 * 4 총 160개입니다. 

#define QUEST_CARDSET_TYPE_BUY_START		(QUEST_CARDSET_TYPE_DINO_TUTORIAL)		// 구입가능 카드셋 시작
#define QUEST_CARDSET_TYPE_BUY_END			(QUEST_CARDSET_TYPE_HUMAN_3RD)			// 구입가능 카드셋 끝

//기존버전에 있는것 모두 삭제할 수는 없다. 아직 파일이 변경되지 않음.. 
#define		MAX_QUEST_PER_CARD				16		//16 한개의 카드가 가질 수 있는 퀘스트
#define		MAX_QUEST_PER_CARDSET			MAX_QUEST_PER_CARD*MAX_CARD_PER_CARDSET							//16 * 10	총 160개입니다.
#define		MAX_QUEST_ALL_USE				MAX_QUEST_PER_CARD*MAX_CARD_PER_CARDSET*MAX_CARDSET_PER_USER	//16 * 10 * 4 총 640개입니다. 

#define		MAX_QUEST_ROW_PER_CARD			4 //카드 한장의 최대 열개수
#define		MAX_QUEST_COL_PER_CARD			4 //카드 한장의 최대 행개수
#define		MAX_QUEST_ITEM_CARD			5 //퀘스트 완료시 지급 가능한 아이템 종류

#define		MAX_QUEST_ARG_COMPLETE	5			// QUEST_CTX 배열중 임무 개수를 담고있는 배열
#define		MAX_QUEST_ARG			6			// QUEST_CTX 배열의 개수입니다.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
// 임무시스템 카드셋
enum ENUM_QUEST_CARDSET_TYPE
{
	QUEST_CARDSET_TYPE_NONE				=	0,
	QUEST_CARDSET_TYPE_TUTORIAL			=	1,		
	QUEST_CARDSET_TYPE_DINO_TUTORIAL	=	2,		//공룡미션 공룡 튜토리얼, QUEST_CARDSET_TYPE_NORMAL는 삭제 됨
	QUEST_CARDSET_TYPE_HUMAN_TUTORIAL	=	3,		//공룡미션 인간 튜토리얼, QUEST_CARDSET_TYPE_SILVER는 삭제 됨
	QUEST_CARDSET_TYPE_GOLD				=	4,		//삭제 됨
	QUEST_CARDSET_TYPE_ASSAULT			=	5,		//돌격
	QUEST_CARDSET_TYPE_BACKUP			=	6,		//지원
	QUEST_CARDSET_TYPE_INFILTRATION		=	7,		//침투
	QUEST_CARDSET_TYPE_SPECIAL			=	8,		//특공
	QUEST_CARDSET_TYPE_DEFCON			=	9,		//데프콘
	QUEST_CARDSET_TYPE_COMMISSIONED_O	=	10,		//사관 
	QUEST_CARDSET_TYPE_COMPANY_O		=	11,		//위관
	QUEST_CARDSET_TYPE_FIELD_O			=	12,		//영관
	QUEST_CARDSET_TYPE_EVENT			=	13,		//이벤트	// 이벤트 카드 13번으로 고정 (20110310) 추가되는것은 이 뒤로 추가하세요.
	QUEST_CARDSET_TYPE_DINO_2ND			=	14,		//공룡미션 공룡 세컨드
	QUEST_CARDSET_TYPE_HUMAN_2ND		=	15,		//공룡미션 인간 세컨드
	QUEST_CARDSET_TYPE_DINO_3RD			=	16,		//공룡미션 공룡 3rd
	QUEST_CARDSET_TYPE_HUMAN_3RD		=	17,		//공룡미션 인간 3rd

	QUEST_CARDSET_TYPE_COUNT,
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)
/////////////////////////////////////////////////////////////////
// 
// 서버에서 저장용, 네트워크 전달용으로 사용합니다. 
//
// Quest의 진행 상태를 저장하는데 사용되는 자료형
//
struct QUEST_ITEM_INFO
{
	//col
	UINT8 m_ColPoint[MAX_QUEST_COL_PER_CARD]; 
	UINT8 m_ColExp[MAX_QUEST_COL_PER_CARD]; 
	UINT8 m_ColPrize[MAX_QUEST_COL_PER_CARD]; 	

	//Row
	UINT8 m_RowPoint[MAX_QUEST_ROW_PER_CARD]; 
	UINT8 m_RowExp[MAX_QUEST_ROW_PER_CARD]; 
	UINT8 m_RowPrize[MAX_QUEST_ROW_PER_CARD]; 	

	//Last
	UINT8 m_Point; 
	UINT8 m_Exp; 
	UINT8 m_Prize; 	
	UINT8 m_AlineDum;
}; 

#pragma pack( push, 4)	// 서버, 클라이언트 동일 파싱 규칙을 적용하기 위해 사용		// mqf파일을 제대로 못읽어들여서 추가합니다. (2015.07.07.수빈)

struct QUEST_ITEM_INFO_NEW
{
	// 2011. 4. 7 수정 - 서동권

	UINT32 m_ui32Point = 0;
	UINT32 m_ui32Exp = 0;
	UINT32 m_ui32Prize = 0;

	struct ItemInfo // 지급할 아이템 구조체
	{
		INT32		m_i32Editor_index = 0;			// ChallengeEditor 에서 사용하기 위한 Index , 툴을 제외한 다른 곳에서는 필요없다.
		INT8		m_i8AuthType = 0;				// 지급할 아이템의 지급 타입 ( 개수제 , 기간제 )
		UINT32		m_ui32ItemID = 0;				// 지급할 아이템의 코드
		UINT32		m_ui32Arg = 0;					// 이 아이템을 몇개를 줄것인가
	};

	ItemInfo m_Item[MAX_QUEST_ITEM_CARD]; // 최대 MAX_QUEST_ITEM_CARD 종류의 아이템까지 지급할 수 있도록
};

struct QUEST_ITEM_INFO_NEW2
{
	QUEST_ITEM_INFO_NEW m_CardReward[MAX_CARD_PER_CARDSET]; // 각 카드별 보상 정보
	QUEST_ITEM_INFO_NEW m_CompleteReward;					// 카드셋 완료 최종 보상 정보

};

#pragma pack( pop )

struct QUESTING_INFO
{
	//ActiveSetInfo
	UINT8			m_ActiveIndexUse = 0;													//진행중인 카드셋 인덱스  0 1 2 3  
	UINT8			m_pActiveIndexCardSet[MAX_CARDSET_PER_USER] = { 0 };						//각 카드셋에서 진행중인 카드번호 0 1 2 3 4 5 6 7 8 9 
	UINT16			m_pActiveIndexCard[MAX_CARDSET_PER_USER][MAX_CARD_PER_CARDSET] = { 0 };		//각 카드의 진행 상태 미성공 0 성공 F

	//카드 사용여부
	UINT8			m_pCardSetType[MAX_CARDSET_PER_USER] = { 0 };								//카드 셋타입	0이면 없음 
};

struct USER_INFO_QUEST
{
	UINT32			_iQuestEventDate = 0;
	UINT8			_pQuestCtxActive[MAX_CARDSET_PER_USER][MAX_QUEST_PER_CARDSET_NEW] = { 0 };
	QUESTING_INFO	_QuestIngInfo;
	UINT32			_pMiniature = 0;
	UINT32			_pInsignia = 0;
	UINT32			_pOrder = 0;
	UINT32			_pMaster = 0;
	UINT64			_ui64GainUserTitle = 0;
	UINT8			_EquipUserTitle[MAX_EQUIP_USER_TITLE] = { 0 };
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif