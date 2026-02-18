#if !defined( __UI_SUB_TAB_CLAN_MGT_INFO_H__)
#define __UI_SUB_TAB_CLAN_MGT_INFO_H__

#include "UITabClan.h"

class UISubTabClanMgt_Info : public UISubTabClan_Mgt
{
	I3_CLASS_DEFINE( UISubTabClanMgt_Info, UISubTabClan_Mgt);
	
public: // Virtual Function
	UISubTabClanMgt_Info();
	virtual	~UISubTabClanMgt_Info();

	virtual void	OnCreate(i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1, void * pArg2) override;
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

public: // Process Function
	void OnClanSendMessage( void);
	void OnInitClanInfo( void);
	void OnModifyClanInfo( void);
	void OnNoticeKillFocus( void);

protected:
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual void _ClearAtUnload(i3UIScene  * pScene) override;

	void _InitUI( void);
	void _CheckIMELength( void);

	void			_GetNoticeMsg( i3::stack_wstring& out );
	void			_GetIntroMsg( i3::stack_wstring& out );
	UINT32			_GetAcceptOption( void);

protected: // Member Variable
	CClanContext * m_pClanContext;

	i3UIEditBox *		m_pEdInputCheck[2];
	i3::wstring			m_wstrInputheck[2];
};

#endif