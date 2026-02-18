#if !defined( __UI_SUB_TAB_CLAN_MGT_CONTROL_H__)
#define __UI_SUB_TAB_CLAN_MGT_CONTROL_H__

#include "UITabClan.h"

class UISubTabClanMgt_Control : public UISubTabClan_Mgt
{
	I3_CLASS_DEFINE( UISubTabClanMgt_Control, UISubTabClan_Mgt);
	
public: // Virtual Function
	UISubTabClanMgt_Control();
	virtual	~UISubTabClanMgt_Control();

	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1, void * pArg2) override;
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	void Apply();
	void UpdateDetailInfo();
	void SetAgeAuthority(UINT8 auth){ m_Management_Control._authority = auth; }
	void SetAgeRank(UINT8 rank);
	void SetAgeUpper(UINT8 age);
	void SetAgeLower(UINT8 age);

protected:
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual void _ClearAtUnload(i3UIScene  * pScene) override;

	
	
protected: // Member Variable
	CClanContext * m_pClanContext;
	CLAN_MANAGEMENT m_Management_Control;

};

#endif