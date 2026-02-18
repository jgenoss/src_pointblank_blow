#pragma once

#include "UIBase.h"

#include "i3Base/itl/signal/signal_common.h"

class iSMenu;
class CharaInfoMgr;

enum SIDEMENU_TYPE
{ 
	SIDEMENU_BTN_NA = -1, 	
	SIDEMENU_BTN_EQUIP = 0, 	
	SIDEMENU_BTN_ABILITY,	
	SIDEMENU_BTN_ALL, 
};

class UISideMenu : public UIBase
{
	I3_CLASS_DEFINE( UISideMenu, UIBase );

public:
	static UISideMenu* instance();

private:
	virtual bool OnRevive( i3RenderContext * pCtx) override;

	typedef void	(UISideMenu::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void	RegisterGameEventFunc( GAME_EVENT evt, call_func f);
	
	void	ReceiveGameEvent_Change_CharaEquip( INT32 arg, const i3::user_data&);
	
	void cb_smenu_ability_close();

public:
	void clicked(int idx);

	void send_change_equip( void);

	//equip 버튼을 클릭 - 
	//arg1 : 반드시 open(true), 열린 상태면 갱신, 안 열린 상태면 skip(false)
	//arg2 : 정보 초기화(true), 비 초기화(false)
	void	smenu_click_equip(bool bOpen, INT32 arg = 0);

public:
	UISideMenu();
	virtual ~UISideMenu();

public:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual void 	OnCreate( i3GameNode * pParent) override;

	virtual void 	OnLoadAllScenes() override;	//UIMainFrame 루틴 - SetEnable(true) 호출시 
	virtual bool 	OnExitStart( void) override;

	virtual void 	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool 	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual bool 	OnPlayClickSound(i3UIControl * pCtrl) override;

	void Entrance();
	void Exit();
	void Notify(const char* msg, void * pArg = nullptr);

public:
	UIMainFrame *	GetMainFrame( void)				{ return ((UIFramework*) m_pParent)->GetUIMainframe(); }
	iSMenu* get_list_ptr(size_t idx) { return mSubList[idx]; }

	bool is_entranced() const	{	return mEntranced;	}

	i3UIFrameWnd* get_frm_wnd(int idx) { return m_FrameWnd[idx]; }
	i3UIButton* get_btn(int idx) { return m_Btn[idx]; }
	i3UIButton* get_current_enable_btn();

	void open_fly_menu();
	void close_fly_menu();

	iSMenu* getSubSidemenu(SIDEMENU_TYPE type) const { return mSubList[type]; }

public:
	void set_change_equip_from_repair_all() { m_bChangeEquipFromRepairAll = true; } //전체 수리할때 장비 교환할 경우 호출한다.
	bool clicked_escape_key();
	REAL32		m_AbilityBtnPosY;
	void		setAbilityButton(bool _enable);

private:
	class CharaInfoMgr *m_pCharaInfoMgr;

	i3UIFrameWnd* m_FrameWnd[SIDEMENU_BTN_ALL];
	i3UIButton* m_Btn[SIDEMENU_BTN_ALL];

private:
	typedef i3::vector< iSMenu* > SubList;
	SubList mSubList;
	bool mEntranced;
	bool m_bChangeEquipFromRepairAll; //전체 수리할때 장비 교환 호출 유무

	i3::scoped_connection_set		m_sigConnection;

};