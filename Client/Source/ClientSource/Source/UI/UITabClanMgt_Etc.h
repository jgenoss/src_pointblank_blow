#if !defined( __UI_TAB_CLAN_MGT_ETC_H__)
#define __UI_TAB_CLAN_MGT_ETC_H__

#include "UITabClan.h"

class UITabClanMgt_Etc : public UITabClan_Mgt
{
	I3_CLASS_DEFINE( UITabClanMgt_Etc);
	typedef UITabClan_Mgt	base;

public:
	UITabClanMgt_Etc();
	virtual	~UITabClanMgt_Etc();

	virtual void	OnCreate(i3GameNode * pParent);
};

#endif