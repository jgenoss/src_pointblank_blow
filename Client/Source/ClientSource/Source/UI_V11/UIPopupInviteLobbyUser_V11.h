#if !defined( __UI_POPUP_INVITELOBBYUSER_V11_H__)
#define __UI_POPUP_INVITELOBBYUSER_V11_H__

#include "UI/UIPopupInviteLobbyUser.h"
#include "UI/UIPopupBase.h"


class UIPopupInviteLobbyUser_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupInviteLobbyUser_V11, UIPopupBase);

private:
	INT32			m_memCount;
	INT32			m_allocCount;
	INVITE_SLOTINFO * m_pUserInfo;
	i3UIListView_Box * m_ListBox;

	i3::vector<INVITE_SLOTINFO*> m_UserList;

protected:
	void			_ResizeUserInfoBuffer( INT32 size);
	INVITE_SLOTINFO * _AllocUserInfo( void);
	INT32			_FindFreeUserInfo( void);
	void			_FreeUserInfo( INT32 idx);
	INT32			_FindUserInfo( const USER_INFO_INVITE * pInfo);

public:
	UIPopupInviteLobbyUser_V11();
	virtual ~UIPopupInviteLobbyUser_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart() override;
	
	void 			UpdateInviteList();

	virtual bool	OnKey_Enter( void) override;

public:
	void	SendInviteMessage();
};

#endif
