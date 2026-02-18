#if !defined( __STEP_PROFILE_CHALLENGE_H__)
#define __STEP_PROFILE_CHALLENGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////
//						임무 카드 페이지
// 2009.01.15
// 작업자 : 정순구
///////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_COMMON_SLOT			4


#define CARD_SLOT_COUNT			10	// 한 세트의 포함된 카드 장수
#define PRIZE_SLOT				4

//// 1급 수훈 약장	14
//// 3급 침투 휘장	8
//// 동성 돌격 훈장 1
//// 특공 마스터 훈장 19

struct CommonSlot
{
	i3GuiButton* _pSlot;
	i3GuiEditBox* _pState;
	i3GuiEditBox * _pNotice;
};

struct CardSlot
{
	i3GuiButton* _pSlot;
	i3GuiStatic* _pCard;
};

#define MAX_PRIZE_SLOT	9



class CGuiNotifyReceiver;
class CStepProfile_CardShopWin;
class CStepProfile_CardInfo;
class CStepProfile_CardCompleteWin;

class CStepProfile_Challenge : public i3ElementBase
{
	I3_CLASS_DEFINE( CStepProfile_Challenge);
protected:
	CGuiNotifyReceiver *		m_pNotifyBase;
	
	i3GuiStatic* m_pChallengeSect;

	i3GuiEditBox* m_pCommonKey;
	CommonSlot m_commonSlot[MAX_COMMON_SLOT];
	

	i3GuiStatic* m_pPrizeNote;

	i3GuiStatic* m_pPrizeRibbonDummy;
	i3GuiStatic* m_pPrizeMedalDummy;
	i3GuiEditBox* m_pPrizeDescription;
	i3GuiEditBox* m_pPrizeNameKey;
	i3GuiEditBox* m_pPrizeNameValue;
	i3GuiEditBox* m_pPrizeExpKey;
	i3GuiEditBox* m_pPrizeExpValue;
	i3GuiEditBox* m_pPrizePointKey;
	i3GuiEditBox* m_pPrizePointValue;
	i3GuiEditBox* m_pCardKey;

	i3GuiButton* m_pCardBg;
	i3GuiStatic* m_pCard;
		
	i3GuiEditBox* m_pCardTypeCombo;
	i3GuiButton* m_pCardTypeButton;	
	i3GuiPopupList* m_pCardTypePopup;

	i3GuiButton* m_pCardBuyOne;
	i3GuiButton* m_pCardBuyAll;
	i3GuiButton* m_pCardDeleteAll;

	CardSlot m_cardSlot[CARD_SLOT_COUNT];
	i3GuiButton* m_pCardInfo;	
	i3GuiEditBox* m_pCcBulletKey;
	i3GuiEditBox* m_pCardBulletKey;
	
	INT32	m_selectedCardSlot;
	INT32	m_selectedCardSet;
	INT32	m_selectedQuest;

	// CardShopWin
	CStepProfile_CardShopWin *	m_pCardShopWin;

	// 임무카드 정보 윈도우
	CStepProfile_CardInfo * m_pCardInfoBG;

	i3GuiStatic* m_pCardInfoWin2;			//큐브웨어 예외처리 
	i3GuiButton* m_pCardInfoConfirm2;		//큐브웨어 예외처리

	// CardComplateWin
	CStepProfile_CardCompleteWin * m_pCardCompleteWin;

	bool	m_bIsMyInfoView;


public:
	BOOL	isEnable( void)					{ return m_pChallengeSect->isEnable(); }
	void	SetEnable( BOOL bEnable);

	void	setMyInfoView( bool bEnable)	{ m_bIsMyInfoView = bEnable; }

public:
	CStepProfile_Challenge();
	virtual ~CStepProfile_Challenge();

	BOOL	LinkControl( CGuiNotifyReceiver * pBase, i3BinList * plist, i3GameNode * pControl);
	void	Init( void);
	void	Update( REAL32 tm);
	void	GuiNotify( I3GUI_CONTROL_NOTIFY* pNotify);

	void	Reset( void);
	void	OnEnd( void);


	void	OnCardSlot(INT32 cardIdx);

	BOOL	OnPopupConfirm( void);
	BOOL	OnPopupCancel( void);

	void	OnShopConfirm(void);
	void	OnShopCancel(void);

	void	OnComplateChallenge( void);
	void	OnComplateConfirm(void);

	void	OnCardInfoConfirm(void);
	void	OnUpdateNote(UINT32 idxChallenge);
	void	OnUpdatePrize( UINT32 idxPrize);
	void	OnCardTypeSelect(INT32 idx);
	void	OnCardBuyAll(void);
	void	OnCardDeleteAll(void);
	void	OnCardInfo(void);
	void	OnCardTypeCombo(void);

	static void CbDeleteCardSet(void* pThis,INT32 nParam);
	static void CbComplateChallenge(void* pThis,INT32 nParam);


	void	OnGameEvent( INT32 event, INT32 arg);

	void	Event_ChangeUserInfo( INT32 iCardSlot = -1);

protected:
	INT32	_FindCardSetIdx(CARDSET_ID cardSetId);
	void	_BuyCardSet(CARDSET_ID cardSetId);
	void	_DeleteCardSet(UINT8 cardSetIdx);
	BOOL	_IsActiveViewCardSet(void);
	void	_ChangeActiveCardSet(CARDSET_ID cardSetId);

	BOOL	_IsPossibilityActiveCard(UINT32 cardIdx);
	BOOL	_CheckBuyQuestCard(INT32 iCardIdx);
	void	_UpdateChallengeSlotState( INT32 iCardSlot);
	void	_UpdateChallengeInfo( INT32 iCardSlot);
	void	_UpdateChallengeCardInfo( INT32 iCardSlot);

	void	_UpdateCardSlotImage( INT32 iSelectedSlot);

	CARDSET_ID	GetSelectedCardsetID();
};

#endif
