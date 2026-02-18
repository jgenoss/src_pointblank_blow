#pragma once

#include "../UI/UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupMissionCardInfo_V10 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionCardInfo_V10, UIPopupBase);

public:
	UIPopupMissionCardInfo_V10();
	virtual ~UIPopupMissionCardInfo_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32* m_CardSetType
	*/
	virtual bool	OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKey_Enter( void) override {    Confirm(); return true;    }
	void			Confirm();

private:
	void	OnCardInfo();

	void _SetCardString(const char* luaName, const i3::wliteral_range& text);
	void _SetCardItem();
	void _SetCardBadge();
	void _SetCardImage();

private:
	i3UIFrameWnd *m_DefaultFrame;
	i3UIFrameWnd *m_EventFrame;

	UIImgSetCtrl* m_ItemSetCtrl;

	i3UIButtonImageSet*	m_CardSlot[MAX_CARD_PER_CARDSET+1];
	i3UIButtonImageSet*	m_DefaultCardImage;
	i3UIButtonImageSet*	m_EventCardImage;
	//INT32 m_EventCardIndex;
//	INT32 m_EventCardMaxCount;
	INT32 m_CardSetType;
};
