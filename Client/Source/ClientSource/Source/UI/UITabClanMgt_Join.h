#if !defined( __UI_TAB_CLAN_MGT_JOIN_H__)
#define __UI_TAB_CLAN_MGT_JOIN_H__

#include "UITabClan.h"

class UITabClanMgt_Join : public UITabClan_Mgt
{
	I3_CLASS_DEFINE( UITabClanMgt_Join);
	typedef UITabClan_Mgt	base;

public:
	UITabClanMgt_Join();
	virtual	~UITabClanMgt_Join();

	virtual void	OnCreate(i3GameNode * pParent);
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();
	virtual void	OnExitEnd();
	virtual void	ProcessGameEvent(INT32 event, INT32 arg);
	virtual void	OnUpdate(REAL32 rDeltaSeconds);
	virtual void	_InitializeAtLoad(i3UIScene * pScene);
	virtual bool	OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */);
	virtual void	OnEntranceEnd(void);

public:
	void	_UpdateClanManagementSubscriberPhase(RT_REAL32);
	void	_UpdateSortSubscriberList();
	void	_SetSubscriptionListSlot(INT32, ManagementSubscriberInfo *);

	void	Click_User_Detail(INT32);
	void	Click_InClan_Accept();
	void	Click_InClan_Reject();
	void	Click_Save_Condition(INT32, INT32, INT32, INT32);
	void	SetUserValue(INT64 *, INT32 *);
	void	AgeLimitCheck( ADMISSION_LIMIT_TYPE type, INT32, INT32);

private:
	void	_SetUserCheckBox(INT32,INT32);	//체크박스
	void	_SetUserIndex(INT32);			//번호
	void	_SetUserRank(INT32,INT32);		//계급
	void	_SetUserNick(INT32,char*);		//닉네임
	void	_SetUserJoinDay(INT32,char*);	//가입일
	void	_SetUserCount(INT32,INT32);		//현재 클랜원수
	void	_SetDefaultComboBox();			//이벤트 완료후 콤보박스 초기화

	void	InitState();
	void	InitAuthority();
	
private:
	CClanContext * m_pClanContext;
	i3UIListView_Box * m_ListBox;
	i3::vector<ManagementSubscriberInfo*>	m_listManageSubscriber;

	ManagementSubscriberInfo m_ManageSubscriberInfo[MAX_REQUEST_COUNT];
};

#endif