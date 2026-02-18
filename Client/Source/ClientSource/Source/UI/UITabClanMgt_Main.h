#if !defined( __UI_TAB_CLAN_MGT_MAIN_H__)
#define __UI_TAB_CLAN_MGT_MAIN_H__

#define USE_CLAN_MARK

#include "UITabClan.h"

class UITabClanMgt_Main : public UITabClan_Mgt
{
	I3_CLASS_DEFINE( UITabClanMgt_Main);
	typedef UITabClan_Mgt	base;
public: // Virtual Function
	UITabClanMgt_Main();
	virtual	~UITabClanMgt_Main();

	virtual void	OnCreate(i3GameNode * pParent);
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();
	virtual void	OnExitEnd();
	virtual void	ProcessGameEvent(INT32 event, INT32 arg);
	virtual bool	OnEntranceStart( void * pArg1, void * pArg2);
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng);
	virtual void	OnUpdate( REAL32 rDeltaSeconds);
	virtual void	OnEntranceEnd();

public: // Process Function
	void OnClanManagementBoardNoticeModify(char *);		//	공지사항 수정완료
	void OnClanManagementBoardGuideModify(char *);		//	클랜소개 수정완료
	void OnClanNameChange();
	void OnClanMarkChange();
	void OnClanSendMessage();
	void OnClanLeave();
	void _InitNotice();
	void _InitIntro();
	void _InitUI();
	void _InitAuthority();
	void _CheckIMELength( void);

protected:
	virtual void _InitializeAtLoad( i3UIScene * pScene);

protected: // Member Variable
	CClanContext * m_pClanContext;

	i3UIEditBox *		m_pEdInputCheck[2];
	char				m_szInputheck[2][MAX_STRING_COUNT];
	BOOL				m_pCheckEditbox;

private:
	INT32 m_authItemFlag;

};

#endif