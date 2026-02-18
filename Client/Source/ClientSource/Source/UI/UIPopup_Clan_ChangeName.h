#if !defined( __UI_POPUP_CLAN_CHANGE_NAME_H__)
#define __UI_POPUP_CLAN_CHANGE_NAME_H__

#if legacy_gui_scene
#include "UIPopupClanBase.h"

class UIPopup_Clan_ChangeName : public UIPopupClanBase
{
	I3_CLASS_DEFINE( UIPopup_Clan_ChangeName, UIPopupClanBase);

public:
	UIPopup_Clan_ChangeName();
	virtual ~UIPopup_Clan_ChangeName();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 INT32* ItemID
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 Arg, const i3::user_data& UserData) override;

	void			_SetCurrentClanName(const char *);
	void			_OnEventDuplicateName(INT32);
	void			_SetDupliButtonEnable(bool);

public:
	void	ChangeName_Confirm();
	void	ChangeName_Cancel();
	virtual bool	OnKey_Enter() { ChangeName_Confirm(); return true; }
	virtual bool	OnKey_Escape() { ChangeName_Cancel(); return true; } 
	
	
	void	ChangeName_Duplicate();
	bool	_CheckNickNameOverLap_Popup();
	char	m_szCurrentName[MAX_STRING_COUNT];
	i3::rc_wstring	m_wstrChangeName;

private:

	T_ItemDBIdx	m_AuthWareDBIndex;
	INT32	ItemID;
	bool	CheckFlag;
	i3UIEditBox * m_EditBox;

};

#endif

#endif

