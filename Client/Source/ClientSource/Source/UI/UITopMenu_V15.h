/**
 * \file	UI\UITopMenu_V15.h
 *
 * Declares the user interface top menu class.
 */

#if !defined( __UI_STATIC_TOP_V15_H__)
#define __UI_STATIC_TOP_V15_H__

#include "UITopMenu.h"
#include "UIFlowEditBox.h"

enum	{ None = -1, Shop, Inven, Gacha, Clan, Myinfo, MailBox, Setting, Back, Exit, GiftShop, MaxCount };

class UITopMenu_V15 : public UITopMenu
{
	I3_CLASS_DEFINE( UITopMenu_V15, UITopMenu);

private:
	struct PushButton
	{
		INT32 i32Idx;
		i3UIButton * pNormalBtn;

		PushButton(void)
		{
			i32Idx = -1;
			pNormalBtn = nullptr;
		}

		void SetPushed()
		{
			pNormalBtn->UnSelectedNormalState();
		}

		bool GetPushed(void)
		{
			return pNormalBtn->isPushFlag();
		}
	};

private:
	REAL32			m_rElapsedBackButtonTime;

	i3UIImageBox *	m_pBGMenu;

	i3UIFrameWnd*			m_mainBtnsFrame;
	i3::vector<PushButton>	m_PushBtnList;

	// Background scene
	i3Node *			m_pCamNode;
	i3Node *			m_pCam[ CAS_MAX];
	CAM_ANIM_STATE		m_CurCameraState;
	CAM_LOOK_CMD		m_CurCameraLook;

	i3Node *			m_pBG;
	i3ReflectMapBindAttr *	m_pRefAttr;

	i3Light *		m_pLight;

public:
	void	cb_tmenu_gauge( bool lastestTarget);
	PBGaugeBar mPBGaugeBar;


protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	// Menu Button을 select Server용으로 바꾼다.
	void			_SetServerMode( void);

	// Menu Button을 select Channel용으로 바꾼다.
	void			_SetChannelMode( void);

	// Menu Button을 일반 Ready용(Lobby, readyroom)으로 바꾼다.
	void			_SetReadyMode( void);

	/** \brief 배경을 로드 */
	void			_LoadBG( void);

	void			_AddLight( void);

	bool			_IsAvailablePush(INT32 value);
	void			_SetPushedButton(INT32 value);

	//------------------------------------------------------------------------------//
	//									inline functions							//
	//------------------------------------------------------------------------------//
public:
	UIMainFrame *	GetMainFrame( void)				{ return ((UIFramework*) m_pParent)->GetUIMainframe(); }

	i3Node *		GetCameraNode( void)			{ return m_pCam[ m_CurCameraState]; }

	i3UIFrameWnd*   GetMainBtnsFrame(void)			{ return m_mainBtnsFrame; }
	i3Texture*		GetUIReflectMap()				{ return m_pRefAttr ? m_pRefAttr->GetTexture() : nullptr; }

public:
	UITopMenu_V15();
	virtual ~UITopMenu_V15();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnClick( i3UIControl * pControl) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void 	OnLoadAllScenes() override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;
	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual void	AllMenuBtnShapeToNormal() override;

	virtual void	SyncSubPhaseWithMenuBtnState() override;

	void SetOriginalCamera();
	void			Resize( void);

	void Clicked(INT32 value);
	void ClickPopupBackButton(bool close_popup = false);	//TopMenu에서 control 하는 menu 만 이 함수로 close 하자.

	bool ClickBackButton( void);
	void ClickExit( void);

	void CloseEnterPopup( void);

	//chunjong.song 2014-07-15
	// UIPopupbase에서 UIPopup_Close함수등에서 TogglePopup/ClosePopup을 이용해서 팝업을 닫을시
	// UITopMenu_V15에 속한 몇개의 팝업들(Mailbox, option)은 별도의 추가작업처리가 되지않아 문제를 유발한다(마지막 팝업탭위치 기억등)
	// UITopMenu_V15에 속한 팝업들도 TolgglePopup/ClosePopup를 이용해서 닫아도 문제없게 하기위해서 아래함수추가
	void			uglyOnMainFrameClosePopupAfter(INT32 popupType);

	/** \brief 쪽지함을 깜박이게 한다. */
	void			StartHighlightMailbox( void);

	////////////////////////////////////////////////////////////////////////
	//						Camera

	void			SetUICharaCamera( CAM_LOOK_CMD state);
	void			UpdateCamera( void);

	virtual bool		ClickInventory( void) override;
protected:
	bool IsServerBasePopup();
	//------------------------------------------------------------------------------//
	//									Callback rutine								//
	//------------------------------------------------------------------------------//
	static void		CbExitNormal(void* pThis,INT32 nParam);
	static void		CbRemoveMercenary(void* pThis,INT32 nParam);
	static void		CbReRegistMercenary(void* pThis, INT32 nParam);


private:

	typedef void	(UITopMenu_V15::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_GET_USER_INFO( INT32, const i3::user_data&);
	void			ReceiveGameEvent_GET_INVEN_INFO( INT32, const i3::user_data&);
	void			ReceiveGameEvent_GET_GIFT_LIST( INT32, const i3::user_data& );
};

#endif
