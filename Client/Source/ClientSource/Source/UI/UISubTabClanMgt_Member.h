#if !defined( __UI_SUB_TAB_CLAN_MGT_MEMBER_H__)
#define __UI_SUB_TAB_CLAN_MGT_MEMBER_H__

#include "UITabClan.h"

// 리스트뷰에 아이템으로 들어가는 템플릿입니다.
// 루아에서 설정이 어려워, 코드에서 설정용도로 사용합니다. EN_양승천(2012.06.21)
#define STAFF_COMBO_BUTTON_SIZE		22
#define	STAFF_COMBO_WIDTH			133
#define STAFF_COMBO_HEIGHT			88
#define STAFF_COMBO_FONTSIZE		9

#define STAFF_BUTTON_WIDTH			74
#define STAFF_BUTTON_HEIGHT			22
#define STAFF_BUTTON_FONTSIZE		9

class UISubTabClanMgt_Member : public UISubTabClan_Mgt
{
	I3_CLASS_DEFINE( UISubTabClanMgt_Member, UISubTabClan_Mgt);

protected:
	void	_UpdateMemberList(void);

	void	_UpdateClanManagementMemberPhase(REAL32);
	void	_UpdateSortClanMemberList(void);

	void	_ChangeMemberLevel(CLAN_MEMBER_LEVEL_TYPE eType, INT64 i64UID);
	void	_DismissalMember( INT64 i64UID);

	bool	_CheckMemberInfo(CLAN_MEMBER_BASIC_INFO * pInfo);

public:
	UISubTabClanMgt_Member();
	virtual	~UISubTabClanMgt_Member();

	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	void			SelectComboItem(INT32 i32Slot, INT32 i32ComboIdx);
	void			ChangeUserLevel(INT32 i32Slot);

public:
	static void		CbSetSlotComboData(i3UIListView_Cell * pCell, void * szData);
	static void		CbSetOKBtn(i3UIListView_Cell * pCell, void * szData);


private:

	void _SetClanMgtMemberInfo(INT32 nSlot, const CLAN_MEMBER_BASIC_INFO * pMember);
	static bool	cbSortMgtMember(const CLAN_MEMBER_BASIC_INFO * pInfo1, const CLAN_MEMBER_BASIC_INFO * pInfo2);

	CClanContext * m_pClanContext;

	typedef i3::shared_ptr<CLAN_MEMBER_BASIC_INFO>		CLAN_MEMBER_BASIC_INFO_PTR;
	i3::vector<CLAN_MEMBER_BASIC_INFO_PTR>	m_vecManageMember;
	i3::vector<UINT8>						m_vecMemberGradeInfo;		// 클랜원 직급 (UI설정용)

};

#endif