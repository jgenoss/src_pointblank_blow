#pragma once

#include "../UI/UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupMissionCardInfo_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionCardInfo_V11, UIPopupBase);

public:
	UIPopupMissionCardInfo_V11();
	virtual ~UIPopupMissionCardInfo_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32*
	*/
	virtual bool	OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKey_Enter( void) override {    Confirm(); return true;    }
	void	Confirm();
	void CardMove(int move_type);


private:
	void	OnCardInfo();
	void	OnShapeChange(void);

	void _SetCardString(const char* luaName, const i3::wliteral_range& text);

	void _SetCardItem();


private:
	i3UIFrameWnd *m_PrizeFrame;
	i3UIFrameWnd *m_MainFrame;
	i3UIButton *m_CloseButton;

	UIImgSetCtrl* m_ItemSetCtrl;
	INT32 m_EventCardIndex;
//	INT32 m_EventCardMaxCount;
	INT32 m_CardSetType;
};
