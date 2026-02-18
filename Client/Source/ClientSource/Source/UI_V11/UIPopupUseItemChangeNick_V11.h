#if !defined( __UI_POPUP_USEITEM_CHAGNENICK_H__)
#define __UI_POPUP_USEITEM_CHANGENICK_H__

#include "UI/UIPopupUseItemBase.h"

class UIPopupUseItemChangeNick_V11 : public UIPopupUseItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemChangeNick_V11, UIPopupUseItemBase);

private:
	i3::rc_wstring m_wstrChangeName;

	i3UIEditBox* m_pEd;

	bool m_bChangeNameChecked;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual LuaState * _SetItemInfo( void);

	void			_UsableNick( bool bEnable);
	void			_ChangeNickOk(void);
	bool			_CheckDuplication( void);

	void			_ChangeNickCheck(void);
	void			_ChangeClanNameOk( void);
	void			_ChangeDisguiseNickOk(void);

public:
	void			ResetChangeNameCheck( void)			{ m_bChangeNameChecked = false; }

public:
	UIPopupUseItemChangeNick_V11();
	virtual ~UIPopupUseItemChangeNick_V11();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData); //부모의 원본 함수 부르지 않습니다. !주위!
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnEntranceEnd( void);

	virtual void OnUpdate( REAL32 rDeltaSeconds);

	virtual bool	OnOKButton( void);
	bool			ChangeNameOK( void);

	bool			DuplicateChangeName( void);

	//------------------------------------------------------------------------------//
	//							Callback routine									//
	//------------------------------------------------------------------------------//
	static void		CbChangeNickWarnning(void* pThis, INT32 nParam);
};

#endif
