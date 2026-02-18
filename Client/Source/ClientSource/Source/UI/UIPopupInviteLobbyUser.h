#if !defined( __UI_POPUP_INVITELOBBYUSER_H__)
#define __UI_POPUP_INVITELOBBYUSER_H__

#include "UIPopupBase.h"

#define INVITE_SLOTCOUNT	7

struct INVITE_SLOTINFO
{
	INVITE_SLOTINFO() : _Chk(0), _Alloc(0), _Deleted(0) {}
	
	void Reset() {	_Info.Reset();	_Chk = 0;	_Alloc = 0;	_Deleted = 0;	}

	USER_INFO_INVITE 	_Info;
	INT32				_Chk;
	INT32				_Alloc;
	INT32				_Deleted;
};

class UIPopupInviteLobbyUser : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupInviteLobbyUser, UIPopupBase);

private:
	INT32			m_memCount;
	INT32			m_allocCount;
	INT32			m_InviteSlotCount;
	INVITE_SLOTINFO * m_pUserInfo;

	i3::vector<INVITE_SLOTINFO*> m_UserList;

protected:
	void			_ResizeUserInfoBuffer( INT32 size);
	INVITE_SLOTINFO * _AllocUserInfo( void);
	INT32			_FindFreeUserInfo( void);
	void			_FreeUserInfo( INT32 idx);
	INT32			_FindUserInfo( const USER_INFO_INVITE * pInfo);

	bool			_IsCheckedAllUser(void);

public:
	UIPopupInviteLobbyUser();
	virtual ~UIPopupInviteLobbyUser();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

	virtual void	OnEntranceEnd( void) override;
	
	void 			UpdateInviteList();

	virtual bool	OnOKButton( void) override;
	virtual bool	OnKey_Enter( void) override;

	bool			SetCheckUser( INT32 slot);
	void			CheckAllUser(bool bChecked);
};

#endif
