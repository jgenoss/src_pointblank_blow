#if !defined( __UI_POPUP_USEITEM_CHAGNENICK_H__)
#define __UI_POPUP_USEITEM_CHANGENICK_H__

#include "UIPopupUseItemBase.h"

class UIPopupUseItemChangeNick : public UIPopupUseItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemChangeNick, UIPopupUseItemBase);

private:
	i3::rc_wstring m_wstrChangeName;

	i3UIEditBox* m_pEd;

	bool m_bChangeNameChecked;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual LuaState * _SetItemInfo( void) override;

	void			_UsableNick( bool bEnable);
	void			_ChangeNickOk(void);
	bool			_CheckDuplication( void);
	
	void			_ChangeNickCheck(void);
	void			_ChangeClanNameOk( void);
	void			_ChangeDisguiseNickOk(void);

public:
	void			ResetChangeNameCheck( void)			{ m_bChangeNameChecked = false; }

public:
	UIPopupUseItemChangeNick();
	virtual ~UIPopupUseItemChangeNick();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnEntranceEnd( void) override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnOKButton( void) override;
	bool			ChangeNameOK( void);

	bool			DuplicateChangeName( void);

	//------------------------------------------------------------------------------//
	//							Callback routine									//
	//------------------------------------------------------------------------------//
	static void		CbChangeNickWarnning(void* pThis, INT32 nParam);
};

#endif
