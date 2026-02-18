/**
 * \file	UI\UITopMenu.h
 *
 * Declares the user interface top menu class.
 */

#pragma  once

#include "UIBase.h"
#include "UI/UIFlowEditBox.h"

enum CAM_ANIM_STATE
{
	CAS_IDLE = 0,
	CAS_IDLE_TO_LEFT,
	CAS_IDLE_TO_RIGHT,
	CAS_LEFT_TO_IDLE,
	CAS_LEFT_TO_RIGHT,
	CAS_RIGHT_TO_IDLE,
	CAS_RIGHT_TO_LEFT,
	
	CAS_MAX
};

enum TOPMENU_MODE
{
	TOPMENU_MODE_NONE = -1,
	TOPMENU_MODE_SERVER = 0,
	TOPMENU_MODE_CHANNEL,
	TOPMENU_MODE_READY,
	TOPMENU_MODE_READYROOM,
	TOPMENU_MODE_SHOP,
};

enum FLOW_STATE
{
	FLOW_STATE_INIT = 0,
	FLOW_STATE_READ_LETTER,
	FLOW_STATE_FLOW,

	FLOW_STATE_MAX
};

enum CAM_LOOK_CMD
{
	CL_IDLE = 0,	// 기본
	CL_SHOP,		// 샾
	CL_WEAPON,		// 무기고

	CL_MAX
};

class UITopMenu : public UIBase
{
	I3_CLASS_DEFINE( UITopMenu, UIBase);

protected:
	TOPMENU_MODE	m_Mode;
	REAL32			m_rElapsedBackButtonTime;
	i3AttrSet *		m_pAttrSet;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene)=0;


public:
	UITopMenu();
	virtual ~UITopMenu();

	void					SetMode( TOPMENU_MODE mode);
	TOPMENU_MODE			GetMode( void) const { return m_Mode; }

	virtual void			Clicked(INT32 value)=0;
	virtual	bool			ClickBackButton( void)=0;
	virtual void			SetUICharaCamera( CAM_LOOK_CMD state)=0;

	virtual void			ClickPopupBackButton(bool close_popup = false)=0;	//TopMenu에서 control 하는 menu 만 이 함수로 close 하자.
	virtual void			StartHighlightMailbox( void)=0;

	virtual void			AllMenuBtnShapeToNormal() = 0;

	virtual void			SyncSubPhaseWithMenuBtnState() = 0;

	//chunjong.song 2014-07-15
	// UIPopupbase에서 UIPopup_Close함수등에서 TogglePopup/ClosePopup을 이용해서 팝업을 닫을시
	// UITopMenu에 속한 몇개의 팝업들(Mailbox, option)은 별도의 추가작업처리가 되지않아 문제를 유발한다(마지막 팝업탭위치 기억등)
	// UITopMenu에 속한 팝업들도 TolgglePopup/ClosePopup를 이용해서 닫아도 문제없게 하기위해서 아래함수추가
	virtual void			uglyOnMainFrameClosePopupAfter(INT32 popupType)=0;
	virtual bool			IsServerBasePopup()=0;

	virtual bool			ClickInventory()=0;

	/** \brief new & hot 버튼 눌렀을 경우 */
	void					GotoShopFromNewAndHot();

	virtual i3AttrSet *		Get3DRoot(void) { return m_pAttrSet; }

protected:
	// Menu Button을 select Server용으로 바꾼다.
	virtual void			_SetServerMode( void)=0;
	// Menu Button을 select Channel용으로 바꾼다.
	virtual void			_SetChannelMode( void)=0;
	// Menu Button을 일반 Ready용(Lobby, readyroom)으로 바꾼다.
	virtual void			_SetReadyMode( void)=0;
	

	//--------------------------------------------------------------------------//
	//									흐르는 공지								//
	//--------------------------------------------------------------------------//
protected:
	UIFlowEditBox	m_FlowEditBox;

	bool			m_bTemporaryNotice;

	i3::rc_wstring	m_strPermanatNotice;
	i3::rc_wstring	m_strTemporaryNotice;

	bool			m_bAlarmCommunity;
	bool			m_bAlarmFlag;
	REAL32			m_rAlarmTerm;

protected:
	void			_UpdateFlowAnnounce( REAL32 rDeltaSeconds);

public:
	/* 쪽지함 버튼을 깜빡이게한다  - 2012.01.18 양승천 */
	void			_UpdateMailBoxAlaram(REAL32 rDeltaSeconds);
	void			_CheckUnReadMail();
	void			_EndHighlightMailbox();

public:
	void			SetSlideNotice_Permanant( const i3::rc_wstring& wstrStr);
	void			SetSlideNotice_Temporary( const i3::rc_wstring& wstrStr);
};

class PBGaugeBar
{
	enum { MAX_IMG_ANI = 16, };

private:
	void update_none(REAL32 rDeltaSeconds) {}
	void update_gauge(REAL32 rDeltaSeconds);

	void resize(REAL32 factor);

public:
	PBGaugeBar() : fn_update(&PBGaugeBar::update_none) {}

	void init(i3UIScene * pScene);

	void reset();
	void set_lastest_target_factor();
	void set_factor(REAL32 unit_factor);
	void update(REAL32 rDeltaSeconds);

public:
	void (PBGaugeBar::*fn_update)(REAL32 rDeltaSeconds);

	i3UIFrameWnd *logo_title;
	i3UIButtonImageSet* logo_imgset;
};