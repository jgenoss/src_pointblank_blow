#if !defined( __UI_POPUP_ADDBLOCKLIST_H__)
#define __UI_POPUP_ADDBLOCKLIST_H__

#include "UIPopupInteractBase.h"
#include "CommunityContext.h"

class UIPopupAddBlockList : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupAddBlockList, UIPopupInteractBase);
private:
	i3UIEditBox *	m_pReasonBox;
	BLOCK_USER		m_BlockUser;
	i3::rc_wstring 	m_wstrComment;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

public:
	UIPopupAddBlockList();
	virtual ~UIPopupAddBlockList();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnOKButton( void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	
	void			OnCancel(void);

	virtual bool	OnKey_Escape() override { OnCancel(); return true; }
public:
	void	SetComment(const i3::rc_wstring& src){ m_wstrComment = src; };
	void	DoBlockUser();

	static void		CbBlockUser( void* pThis, INT32 nParam);
};

#endif
