#if !defined( __UI_TAB_CLAN_MGT_MEMBER_H__)
#define __UI_TAB_CLAN_MGT_MEMBER_H__

#include "UITabClan.h"

class UITabClanMgt_Member : public UITabClan_Mgt
{
	I3_CLASS_DEFINE( UITabClanMgt_Member);
	typedef UITabClan_Mgt	base;
public:
	UITabClanMgt_Member();
	virtual	~UITabClanMgt_Member();

	virtual void	OnCreate(i3GameNode * pParent);
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();
	virtual void	OnExitEnd();
	virtual void	ProcessGameEvent(INT32 event, INT32 arg);
	virtual void	OnUpdate(REAL32 rDeltaSeconds);

	void			InitState();
	void			InitCheckBox();
	void			InitAuthority();
	
	CLAN_DETAIL_INFO * GetMyClanDetailInfo(void);
	void	_UpdateClanManagementMemberPhase(REAL32);
	void	_UpdateSortClanMemberList(void);
	void	_SetManagementListSlot(INT32, ManagementClanMemberInfo *);

	void	OnManagementInstructionPopup(INT32 nItemIdx);
	void	OnManagementexpellend();
	void	OnManagementStaffAuthModify();

	void	_SetUserIndex(INT32);
	void	_SetNameUsingColorTable(INT32, char *pszName, UINT8 color);
	void	_SetUserClass(INT32, char *);
	void	_SetEntryDay(INT32, char *);
	void	_SetRankImage(INT32, INT32);
	void	_SetConnectState(INT32, char *);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
private:
	BOOL	SetUserValue(INT64 *, INT32 *);

private:
	CClanContext * m_pClanContext;
	i3UIListView_Box * m_ListBox;
	i3::vector<ManagementClanMemberInfo*>	m_listManageMember;

	ManagementClanMemberInfo m_ManageMemberInfo[MAX_MEMBER_COUNT];
	ManagementSubscriberInfo m_ManageSubscriberInfo[MAX_MEMBER_COUNT];

	i3UICheckBox *Box[STAFF_AUTHORITY];
};

#endif