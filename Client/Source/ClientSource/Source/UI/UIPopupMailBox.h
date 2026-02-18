/**
 * \file	UI\UIPopupMailBox.h
 *
 * Declares the user interface popup mail box class.
 */


#if !defined( __UI_POPUP_MAILBOX_H__)
#define __UI_POPUP_MAILBOX_H__

#include "UIPopupBase.h"
#include "../CommunityContext.h"

struct NOTE_RECEIVE;

class UITabMailBox;

class UIPopupMailBox : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMailBox, UIPopupBase );

public:
	enum MAILBOXTAB
	{
		TAB_NONE			= -1, 
		TAB_MESSAGE_LIST	= 0,
		TAB_GIFT_LIST,

		MAX_MAILBOXTAB,
	};

private:	
	UITabMailBox *			m_pMailBoxTap[MAX_MAILBOXTAB];
	MAILBOXTAB				m_eCurrentTab;
	MAILBOXTAB				m_eNextTab;

	UIMessageBox *			m_pMsgBoxDeleteCallback;

public:
	bool		ChangeTab( int nTab );
	void		ProcessNextTab( void );
	void		SetNoticeMessage(const i3::wliteral_range& wNoticeRng);

	void		SetSelectAll( bool bSelect );
	void		DeleteMail(void);
	void		SetReadedMail(void);
	void		SendMail(void);
	void		ReceiveAllItem(void);
	
	void		StartAlarmNote(void);

	UITabMailBox *	GetTab(MAILBOXTAB eTab)	{ return m_pMailBoxTap[eTab]; }
	UITabMailBox *	GetCurrentTab(void)
	{
		if( m_eCurrentTab > -1)
			return m_pMailBoxTap[m_eCurrentTab];
		return nullptr;
	}

protected:
	void			_SetButtonState(void);
	void			_SetEnableGiftTap(bool bEnable);

public:
	virtual void		OnCreate( i3GameNode* pParent ) override;

	virtual bool		OnEntranceStart( void* pArg1 = nullptr, void* pArg2 = nullptr ) override;
	virtual void		OnEntranceEnd() override;

	virtual bool		OnExitStart(void) override;

	virtual void		ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData ) override;

	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;
protected:
	virtual void		_ClearAtUnload( i3UIScene* pScene ) override;

public:
	UIPopupMailBox();
	~UIPopupMailBox();

	inline void			Reset_DeleteCallback_MsgBox(void)	{ m_pMsgBoxDeleteCallback = nullptr; }
	static void			cbDeleteMail(void* pThis,INT32 nParam);
};

#endif
