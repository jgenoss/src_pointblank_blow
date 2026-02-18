#if !defined( __UI_POPUP_ADDBLOCKLIST_H__)
#define __UI_POPUP_ADDBLOCKLIST_H__

#include "../UI/UIPopupInteractBase.h"
#include "../CommunityContext.h"

class UIPopupAddBlockList_V11 : public UIPopupInteractBase
{
	typedef UIPopupInteractBase Base;
	I3_CLASS_DEFINE( UIPopupAddBlockList_V11, UIPopupInteractBase);
private:
	BLOCK_USER		m_BlockUser;
	i3UIEditBox *	m_pCommentBox;
	//const char*			m_Comment;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

public:
	UIPopupAddBlockList_V11();
	virtual ~UIPopupAddBlockList_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnOKButton( void) override;
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	
	void			OnCancel(void);

	virtual bool	OnKey_Escape() override { OnCancel(); return true; }
public:
	void	DoBlockUser();

	static void		CbBlockUser( void* pThis, INT32 nParam);
};

#endif
