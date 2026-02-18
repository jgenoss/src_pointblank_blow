/**
 * \file	UI\UITabClanMember.h
 *
 * Declares the user interface tab clan member class.
 */


#if !defined( __UI_TAB_CLAN_MANAGEMENT_H__)
#define __UI_TAB_CLAN_MANAGEMENT_H__

#include "UITabClan.h"

enum UISubTabClanMgtType
{
	USTC_MGT_NONE = -1,
	USTC_MGT_JOIN,		// 가입 0
	USTC_MGT_GRADE,		// 등급 1
	USTC_MGT_INFO,		// 정보 2
	USTC_MGT_CONTROL,	// 기타관리 3

	USTC_MGT_MAX,
};

class UITabClanManagement : public UITabClan_Normal
{
	I3_CLASS_DEFINE( UITabClanManagement, UITabClan_Normal);

private:
	UISubTabClan_Mgt * m_pClanMgtTab[USTC_MGT_MAX];

	UISubTabClanMgtType m_eCurrentTab;
	UISubTabClanMgtType m_eNextTab;

public:
	UITabClanManagement();
	~UITabClanManagement();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart( void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	void	ChangeSubTab(UISubTabClanMgtType eTab);
	void	ProcessNextTab(void);

	UISubTabClan_Mgt * GetTab( UISubTabClanMgtType tab) { return m_pClanMgtTab[tab]; }
	UISubTabClan_Mgt * GetCurrentTab(void) { return m_pClanMgtTab[m_eCurrentTab]; }
};

#endif	//__UI_TAB_CLAN_MANAGEMENT_H__