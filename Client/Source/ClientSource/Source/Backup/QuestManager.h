#if !defined( _CQUEST_MANAGER_H__)
#define _CQUEST_MANAGER_H__

#include "../../CommonSource/QuestDef.h"
#include "QuestBase.h"

#include "CardInfo.h"
#include "CardSet.h"

class CCardSet;
class CCardInfo;

typedef struct _tagQuestInfo
{
	UINT8			nidx;		// 각 4개의 Row중의 퀘스트 인덱스
	UINT16			ctxidx;		// QuestCtx중의 인덱스
	bool			isComplete;	// 현재 완료인지 여부
	CQuestBase	*	pQuest;		// 퀘스트
}QUESTINF;


class CQuestManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CQuestManager, i3ElementBase);

protected:
	i3::vector<CCardSet*>				m_CardSet;	// CCardSet

	//현재 수행중인 멤버에 대한 정보 입니다. 
	QUESTINF			m_QuestData[MAX_QUEST_COL_PER_CARD];	//수행중인 퀘스트에대한 정보	
	CCardSet	*		m_pCurrentCardSet;						//현재 수행중인 카드셋

	bool				m_bNeedUpdate;

	//화면에 보상에 관련된 부분을 보여주기 위한 변수 
	BOOL				m_bGainedPrizeByBattle;			// 전투 완료 후 보상팝업을 위한 테이블
	INT32				m_gainedPrizeByBattle;			// 전투 완료 후 
	INT32				m_GainedTotalExp;				// 전투 완료 후 얻는 포인트
	INT32				m_GainedTotalPoint;				// 전투 완료 후 얻는 포인트 
	INT32				m_CompletedCardIndex;

	BOOL				m_bCompleteQuestByBattle;		// 전투 완료 후 퀘스트 완료여부

private:
	// Active CardSet INDEX, 현재 active 상태인 CARD_SET의 index - 현재는 Maximum 4개
	UINT8				m_ActiveIndex;

public:
	void					setQuestContext( const QUEST_CTX & ctx, INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex );
	void					setQuestComplete( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex, UINT8 Value );

	void					setQuestItemInfo( INT32 CardSetIndex, INT32 CardIndex, const QUEST_ITEM_INFO_NEW & ItemInfo );

public:
	CQuestManager(void);
	virtual ~CQuestManager(void);

	bool				BuildQuestData(QUESTING_INFO * pQuestAllInfo);
	bool				Reset();


protected:
	bool				_SetActiveCard(void);			//카드셋이 변경되면 호출하여 카드셋이 가지고있는 값대로 현 매니저에 정리한다.
	bool				_SetActiveQuest(void);			//카드가 변경되면 호출하여 카드가 가지고 있는 값대로 현 매니저에 정리한다.
	bool				_SetActiveCardSet(void);		//현재 카드셋의 값을바탕으로 현재 매니저에 정리

	bool				_RestoreQuest(void);			//진행된 퀘스트를 서버에 있는값으로 되돌리기위함( 스테이지 강제퇴장의 경우 진행되지 않은 상태로 돌려야함)
	
	void				_CheckGainedPrizeByBattle(void);		// 전투후 보상팝업용 테이블 값 계산

	INT32				_GetNextAvailableCardIndex(void);

public:
	INT32				_SetNextAvailableCardSetIndex(void);
	UINT8				getActiveCardSetIndex( void);
	UINT8				getActiveCardIndex( void);

	bool				ChangeActiveCardSet( UINT32 idx);			//active card set을 변경
	bool				ChangeActiveCard( UINT32 idx);				//active card를 변경
	bool				IsAvailableCard( INT32 idx);
	bool				CanSelectCard( INT32 idx);

public:
	bool				HasQuest( UINT32 Index );

	//get active
	bool				IsActivedCardset()				{ return m_pCurrentCardSet != NULL; }
	CCardInfo	*		getActiveCard( void);
	CQuestBase	*		getActiveQuest( UINT32 nRow)	{ I3_BOUNDCHK( nRow, MAX_QUEST_PER_CARD_NEW); return m_QuestData[nRow].pQuest;}

	//find card
	CCardInfo	*		FindCard( UINT32 idxCard, UINT32 idxCardSet);
	CQuestBase	*		FindQuest( UINT32 idxQuest, UINT32 idxCard, UINT32 idxCardSet);

	BOOL				isCompleteQuest(UINT32 iIdx);
	BOOL 				isNowCompleteQuest(UINT32 iIdx);
	bool				isQuestActivated(void);

	void				GetQuestQueryDesc(char* pszBuff,INT32 idxQuest,INT32 maxLen);
	bool				GetQuestQueryHUDDesc(char* pszBuff,INT32 idxQuest,INT32 maxLen);
	bool				IsCompleteActiveCardSet(void);
	bool				IsCompleteActiveCard(void);
	bool				IsCompletedCard(INT32 CardIndex);
	BOOL				BePrize();

	//
	bool				NoticeChangeActiveIndex();	//현재 active index들의 상태를 서버에 보냄
	void				ResetGainedPrizeByBattle(void);			// 전투후 보상팝업용 테이블 초기화
	BOOL				BeGainedPrizeByBattle(void);			// 전투후 보상팝업용 테이블에 보상정보 있는지 확인
	INT32				GetGainedPrizeByBattle(void);
	BOOL				BeCompleteQuestByBattle(void);			// 전투 중 완료된 퀘스트가 존재하는가

	INT32				getGainedTotalExp(void)				{	return m_GainedTotalExp;}
	INT32				getGainedTotalPoint(void)			{	return m_GainedTotalPoint;}
	INT32				getCompletedCardIndex(void)			{ return m_CompletedCardIndex;}

public:
	void				OnUpdate( RT_REAL32 rDeltaSeconds);
	bool				OnGiveUpBattle(void);
	bool				OnStageStart();		//(STAGE_INFO * pStage);
	bool				OnRoundStart();     //(STAGE_INFO * pStage);
	bool				OnStageEnd();		// (STAGE_INFO * pStageInfo);
	
public:
	bool				AddQuestFinishToHud();
	bool				UpdateQuestStateBoard();
	BOOL				CheckCompleteCard(void);

	QUESTINF	*		getQuestData(INT32 idx)	{	I3_BOUNDCHK( idx, MAX_QUEST_PER_CARD_NEW); return &m_QuestData[idx];}
	bool				RemoveActiveCardset();

	bool				HasCardSet(CARDSET_ID CardsetID);
	bool				HasCard(INT32 Index);
	bool				HasEventCardset();
	INT32				GetCardsetIndex(CARDSET_ID CardsetID);
	bool				SetFirstCardsetActive();
	CARDSET_ID			GetActiveCardsetID();

	bool				GetCardReward(UINT8 & Prize, UINT8 & Point, UINT8 & Exp);

public:
	//2011년 3월 18일 이정우
	//미션 카드 미리 보기에 필요한 메소드
	CCardSet*		_getCardSet( UINT32 idx);	
	CCardSet*		_getActiveCardSet()	{	return m_pCurrentCardSet;	}

private:
	CCardSet *		getCardSet( UINT32 idx);	
	CCardSet *		AcquireCardSet( INT32 CardSetIndex );	// 없으면 만들고 있으면 반환
	CCardInfo *		getCard( UINT32	idx);
	CQuestBase *	getQuest( UINT32 idxQuest);

	UINT32			getActiveQuestIndex( UINT32 nIdx);
	CCardSet *		getActiveCardSet( void)			{ return m_pCurrentCardSet;}

	bool			RemoveQuestCard( UINT32 CardSetIdx );


	void			InitData();

	// 임무카드 서버로 이전후 생긴 함수
public:
	bool			Process_PROTOCOL_BASE_QUEST_CHANGE_ACK(UINT8 ui8FinishFlagAndProcessedQuestIdx, UINT8 ui8ProcessedTaskCount);
	void			Process_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK();
	void			SaveQuest();

public:
	const char * GetTemplateName(ENUM_QUEST_CARDSET_TYPE CardType);
	INT32 GetTemplateIndex(ENUM_QUEST_CARDSET_TYPE CardType);
};



INT32 CalculateQuestCardIndexInArray( INT32 CardSetIndex, INT32 CardIndex, INT32 QuestIndex );

#endif
