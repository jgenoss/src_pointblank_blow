#if !defined( __UI_SUB_TAB_CLAN_MGT_JOIN_H__)
#define __UI_SUB_TAB_CLAN_MGT_JOIN_H__

#include "UITabClan.h"

#define STAFF_BUTTON_WIDTH			74
#define STAFF_BUTTON_HEIGHT			22
#define STAFF_BUTTON_FONTSIZE		9

class UISubTabClanMgt_Join : public UISubTabClan_Mgt
{
	I3_CLASS_DEFINE( UISubTabClanMgt_Join, UISubTabClan_Mgt);

public:
	UISubTabClanMgt_Join();
	virtual	~UISubTabClanMgt_Join();

	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */) override;
	virtual void	OnEntranceEnd(void) override;


public:
	void	OnJoinAccept(INT32 i32Slot);
	void	OnJoinReject(INT32 i32Slot);

	static void	CbSetSlotAcceptBtn(i3UIListView_Cell * pCell, void * szData);
	static void	CbSetSlotRejectBtn(i3UIListView_Cell * pCell, void * szData);

	static void	SetSlotBtnData(i3UIListView_Cell * pCell, const i3::rc_wstring& wstrText);

private:
	void	_OnUpdateSubscriber(REAL32 rDeltaSeconds);
	void	_UpdateSubscriberList(void);
	void	_SortSubscriberList( void);

	void	_SetSlotData(INT32 idx, CLAN_REQUEST_BASIC_INFO* pSubscriber);

private:
	typedef i3::shared_ptr<CLAN_REQUEST_BASIC_INFO>	CLAN_REQUEST_BASIC_INFO_PTR;

	CClanContext * m_pClanContext;
	
	i3::vector<CLAN_REQUEST_BASIC_INFO_PTR>	m_vecManageSubscriber;

};

#endif