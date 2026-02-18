/**
 * \file	UI\UITopMenu_V10.h
 *
 * Declares the user interface top menu class.
 */

#if !defined( __UI_STATIC_TOP_V10_H__)
#define __UI_STATIC_TOP_V10_H__

#include "UI/UITopMenu.h"
#include "UI/UIFlowEditBox.h"
#include "UI/UIFramework.h"


enum	{ None = -1, Youtube, Clan, Missioin, Inven, Shop, Community, Setting, Exit, MaxCount };

class UITopMenu_V10 : public UITopMenu
{
	I3_CLASS_DEFINE( UITopMenu_V10, UITopMenu);
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

	i3::vector<PushButton>	m_PushBtnList;
	i3UIScene *		m_pBottom;
	bool			m_bCommunity;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	// Menu Button을 select Server용으로 바꾼다.
	virtual void			_SetServerMode( void) override;
	// Menu Button을 select Channel용으로 바꾼다.
	virtual void			_SetChannelMode( void) override;
	// Menu Button을 일반 Ready용(Lobby, readyroom)으로 바꾼다.
	virtual void			_SetReadyMode( void) override;


	const char* _GetCameraAnimationFileName(CAM_ANIM_STATE state) const;

	void			_LoadBG( void);

	void			_AddLight( void);

	//------------------------------------------------------------------------------//
	//									inline functions							//
	//------------------------------------------------------------------------------//
public:
	UIMainFrame *	GetMainFrame( void)				{ return ((UIFramework*)m_pParent)->GetUIMainframe(); }

public:
	UITopMenu_V10();
	virtual ~UITopMenu_V10();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void 	OnLoadAllScenes() override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;

	virtual bool	OnKey_Enter( void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual void	AllMenuBtnShapeToNormal() override;
	virtual void    SyncSubPhaseWithMenuBtnState() override;

	void			ResizeMainScene( void);

	void			SetYouTubeButton(void);

private:
	bool	m_bCallOverlap;

	//------------------------------------------------------------------------------//
	//										Menu Bar								//
	//------------------------------------------------------------------------------//

public:
	virtual void		Clicked(INT32 value) override;
	virtual void		ClickPopupBackButton(bool close_popup = false) override {}
	virtual	bool		ClickBackButton( void) override;

	virtual void		StartHighlightMailbox(void) override;

	virtual void		uglyOnMainFrameClosePopupAfter(INT32 popupType) override {}
	virtual bool		IsServerBasePopup() override { return false;}

	void				SetEnableCommunity(bool enable);

private:
	bool				ClickYoutube();
	bool				ClickClan();
	bool				ClickMission();
	virtual bool		ClickInventory( void) override;
	bool				ClickShop( void);
	void				ClickCommunity( void);
	void				ClickOption( void);
	void				ClickExit( void);

	void				_SetPushedButton_V10();

	INT32				m_iPrePushBtn_idx;

protected:
	MultiSceneInfo	m_main_scn_info, m_myinfo_scn_info;

private:
	i3UIButtonImageSet * m_pNationFlag;

	//------------------------------------------------------------------------------//
	//									TS GaugeBar									//
	//------------------------------------------------------------------------------//
public:
	void	cb_tmenu_gauge( bool lastestTarget);

public:
	PBGaugeBar		mPBGaugeBar;


	//------------------------------------------------------------------------------//
	//									Show Room									//
	//------------------------------------------------------------------------------//
public:
	virtual void		SetUICharaCamera(CAM_LOOK_CMD state) override;

	void				UpdateCamera(void);
	void				SetOriginalCamera();

private:
	i3Node *				m_pBG;
	i3Light *				m_pLight;
	i3Node *				m_pCamNode;
	i3Node *				m_pCam[ CAS_MAX];
	i3ReflectMapBindAttr *	m_pRefAttr;
	CAM_ANIM_STATE			m_CurCameraState;
	CAM_LOOK_CMD			m_CurCameraLook;

	i3UIButton*				m_pShopButton;


	//------------------------------------------------------------------------------//
	//								       MyInfo									//
	//------------------------------------------------------------------------------//
public:
	void			UpdateMyInfo(void);

private:

	void			_SetMyDefaultInfo();
	void			_SetMyNationFlag(void);
	void			_SetUserExp();
	void			_SetMoney();

public:
	void			InitMyInfo(void);

	//------------------------------------------------------------------------------//
	//							Received Game Eent Functions						//
	//------------------------------------------------------------------------------//
private:
	typedef void	(UITopMenu_V10::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Update_MyInfo( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_GiftGoods(INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_ReceiveNote(INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_IngameURL(INT32, const i3::user_data&);


	//------------------------------------------------------------------------------//
	//									Callback rutine								//
	//------------------------------------------------------------------------------//

	static void		CbExitNormal(void* pThis,INT32 nParam);
	static void		CbRemoveMercenary(void* pThis, INT32 nParam);
	static void		CbReRegistMercenary(void* pThis, INT32 nParam);

};

#endif
