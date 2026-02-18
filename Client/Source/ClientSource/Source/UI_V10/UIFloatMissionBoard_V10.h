#pragma once

#include "../UI/UIFloatingBase.h"

struct MainTitle_change_SizeUI
{
	REAL32 back_frame_marzine;

	i3UIFrameWnd *  m_pTitleBack;
	i3UIButton	 *  m_pTitleBackButton;

	MainTitle_change_SizeUI()
	{
		back_frame_marzine  = 10.0f;
		m_pTitleBack		= nullptr;
		m_pTitleBackButton  = nullptr;
	}

	void setWidth(REAL32 rwidth)
	{
		//해당 UI가 없을 수 있다.
		if (m_pTitleBack == nullptr || m_pTitleBackButton == nullptr)
			return;

		m_pTitleBack->setSize( rwidth - back_frame_marzine, m_pTitleBack->getHeight());
		m_pTitleBackButton->setSize( rwidth, m_pTitleBack->getHeight());
	}	

	void setBackFrame(i3UIFrameWnd * pframe)  { m_pTitleBack = pframe; }
	void setBackButton(i3UIButton  * pbutton) { m_pTitleBackButton = pbutton; }
};

class UIFloatMissionBoard_V10 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMissionBoard_V10, UIFloatingBase);

public:
	UIFloatMissionBoard_V10();
	virtual ~UIFloatMissionBoard_V10();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

public:
	void			SetMissionCardSlot();
	void			Click(INT32 idx);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

private:
	void			_GetUIControlSize();
	void			_SetUIControl(INT32 count);

private:
	REAL32			m_BastTextHeight;
	REAL32			m_TextHeight;

	REAL32			m_BaseButtomBoxPosY;
	REAL32			m_ButtomBoxPosY;

	i3UIStaticText * m_apText[4];
	i3UIStaticText * m_pTitleText;
	i3UIStaticText * m_pHideTitleText;

	i3UIImageBox *	m_pButtomBoxImage;

	i3UIFrameWnd *  m_pMainWnd;
	i3UIFrameWnd *	m_pMissionBoardWnd;
	i3UIFrameWnd *	m_pHideBoardWnd;
	
	MainTitle_change_SizeUI m_ChangeUI[2];

	REAL32			m_origMinTextWidth;
	REAL32			m_origMinFrameWidth;
};