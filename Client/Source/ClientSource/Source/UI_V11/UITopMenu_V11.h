/**
 * \file	UI\UITopMenu_V11.h
 *
 * Declares the user interface top menu class.
 */

#if !defined( __UI_STATIC_TOP_V11_H__)
#define __UI_STATIC_TOP_V11_H__

#include "UI/UITopMenu.h"
#include "UI/UIFlowEditBox.h"
#include "UI/UIFramework.h"

enum	{ None = -1, Shop, Inven, Gacha, Clan, Myinfo, MailBox, Setting, Back, Exit, GiftShop, MaxCount };

class UITopMenu_V11 : public UITopMenu
{
	I3_CLASS_DEFINE( UITopMenu_V11, UITopMenu);
private:
	i3UIScene *		m_pBottom;

	// 레디룸 정보 세팅
	i3UIEditBox *	m_pRoomInfo;
	void			_SetRoomInfo();
	void			_ResetRoomInfo();

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	// Menu Button을 select Server용으로 바꾼다.
	virtual void			_SetServerMode( void) override;
	// Menu Button을 select Channel용으로 바꾼다.
	virtual void			_SetChannelMode( void) override;
	// Menu Button을 일반 Ready용(Lobby, readyroom)으로 바꾼다.
	virtual void			_SetReadyMode( void) override;

public:
	INT32 m_mainSceneIdx;

	void	cb_tmenu_gauge(bool lastestTarget);
	PBGaugeBar mPBGaugeBar;

	//------------------------------------------------------------------------------//
	//									inline functions							//
	//------------------------------------------------------------------------------//
public:
	UIMainFrame *	GetMainFrame( void)				{ return ((UIFramework*)m_pParent)->GetUIMainframe(); }
	i3UIScene *		GetBottom( void)				{ return m_pBottom; }

public:
	UITopMenu_V11();
	virtual ~UITopMenu_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void 	OnLoadAllScenes() override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;

	virtual bool	OnKey_Enter( void) override;

	virtual void	AllMenuBtnShapeToNormal() override;

	virtual void	SyncSubPhaseWithMenuBtnState() override;

	void			Resize( void);

	/** \brief 서버-채널-Room string */
	void			OnModifyCurrentPosition( const char * pszStr);

	void			ClearTabSelection() const;

	bool			ClickClan();
	virtual bool		ClickInventory( void) override;
	bool			ClickShop( void);
	bool			ClickGiftShop(void);
	bool			ClickRouletteShop( void);
	void			ClickMailBox( void);
	void			ClickOption( void);
	void			ClickExit(void);
	void			_SetPushedButton_V11();
	void			SetBtnResetState(bool bstate) { m_bBtnReset = bstate; }

private:
	INT32			m_nCurrentCountDown;	
	POINT32			m_ExitMsgBoxOffset;
	REAL32			m_bFlowTime;
	bool			m_bBtnReset;

public:
	virtual void		Clicked(INT32 value) override;
	virtual bool		ClickBackButton( void) override;
	virtual void		ChangeCamera( CAM_LOOK_CMD state) {}
	virtual void		ClickPopupBackButton(bool close_popup = false) override {}
	virtual void		SetUICharaCamera(CAM_LOOK_CMD state) override {};

	/** \brief 쪽지함을 깜박이게 한다. */
	virtual void		StartHighlightMailbox( void) override;

	virtual void		uglyOnMainFrameClosePopupAfter(INT32 popupType) override {}
	virtual bool		IsServerBasePopup() override { return false;}
	

	//------------------------------------------------------------------------------//
	//									Callback rutine								//
	//------------------------------------------------------------------------------//

	static void		CbExitNormal(void* pThis,INT32 nParam);
	static void		CbRemoveMercenary(void* pThis, INT32 nParam);
	static void		CbReRegistMercenary(void* pThis, INT32 nParam);
};

#endif
