#if !defined( __UI_SUB_TAB_CLAN_MGT_ETC_H__)
#define __UI_SUB_TAB_CLAN_MGT_ETC_H__

#include "UITabClan.h"

class UISubTabClanMgt_Etc : public UISubTabClan_Mgt
{
	I3_CLASS_DEFINE( UISubTabClanMgt_Etc, UISubTabClan_Mgt);

public:
	UISubTabClanMgt_Etc();
	virtual	~UISubTabClanMgt_Etc();

	virtual void	OnCreate(i3GameNode * pParent) override;
};

#endif