#pragma once


// 구버전 쪽 1.0/1.1 미션카드 UI 모두 쓰레기같이 너덜한데, v3로 옮기고 분석해서 다소 줄여낸 편이지만..여전이 줄일게 너무 많은 상태다.....
// 좀더 분석이 필요함..

// 기존코드가 함수간 쓸데없는 중복호출이 극심해서 정리 더 하는 중...
// 미션카드는 선택UI외에 정보팝업, 구매팝업, HUD 처리가 더 있기 때문에 양이 많고, 아직 처리되지 않음..

#include "UITabProfileBase_V11.h"

class UITabProfileMissionCard_V11 : public UITabProfileBase_V11
{
	I3_CLASS_DEFINE( UITabProfileMissionCard_V11, UITabProfileBase_V11);
	typedef UITabProfileBase_V11		base;
private:

	INT32			m_selectedCardSetIndexFromList;		// 이미 구매된 세트 슬롯(4)과 카드셋 리스트(10개이상)와의 구별이 필요함..
	INT32			m_nextSetTypeForDelete;

	i3UIButtonImageSet*	m_CardSlot[MAX_CARD_PER_CARDSET];
	i3UIButton*		m_CardBG[MAX_CARD_PER_CARDSET];
	i3UIListView_Box*   m_pCardSetListView;
	
	void			UpdateCardSlotsAndInfoUI();
	void			UpdateCardSetListSelection();

public:
	UITabProfileMissionCard_V11();
	virtual ~UITabProfileMissionCard_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE ) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	/** \brief 임무 보상 Update */
	void			UpdatePrize_Lua();
	void			SelectCardSetType_Server( INT32 serverIdx);
	void			SelectCard( INT32 cardIdx);
	void			InitCardList_Lua(void);

	//Luaglue function Link
	void			CardInfo();
	void			CardBuy();
	void			CardDelete();
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
protected:
	void			_ChangeActiveCardSet(INT32 cardSetType);
	void			_UpdateChallengeSlotState();
	void			_UpdateChallengeCardInfo();
	void			_UpdateCardSlotImage();
private:
	void			SetButtonState(i3UIButton *, I3UI_BUTTON_SHAPE);

	//카드 상세정보를 갱신
	void			SetCCardNotice(INT32, const i3::rc_wstring& wstrText);
	void			SetCCardNoticeColor(INT32, INT32);
	void			SetCCardNoticeEnable(INT32, bool);
	void			SetCCardNoticeCheck(INT32,INT32);
	//이벤트 필터
	void			SetMissionPossibleCount_Lua();
};
