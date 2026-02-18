#if !defined( __UI_POPUP_CLAN_CHANGE_MARK_H__)
#define __UI_POPUP_CLAN_CHANGE_MARK_H__

#include "UIPopupBase.h"

class UIPopup_Clan_ChangeMark : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopup_Clan_ChangeMark);

public:
	UIPopup_Clan_ChangeMark();
	virtual ~UIPopup_Clan_ChangeMark();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	ProcessGameEvent(UINT32 event, INT32 Arg);

public:
	void	ChangeMark_Confirm();
	void	ChangeMark_Cancel();

};

#endif
