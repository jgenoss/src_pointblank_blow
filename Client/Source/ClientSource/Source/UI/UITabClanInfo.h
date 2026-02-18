#if !defined( __UI_TAB_CLAN_INFO_H__)
#define __UI_TAB_CLAN_INFO_H__

#include "UITabClan.h"

enum UI_CBMS
{
	UI_CBMS_EXP		= 0,	
	UI_CBMS_KILL_COUNT,
	UI_CBMS_HEAD_COUNT,	
	UI_CBMS_CONNECTING,
	UI_CBMS_WIN_COUNT,	

	UI_CBMS_MAX
};

class UITabClanInfo : public UITabClan_Normal
{
	I3_CLASS_DEFINE( UITabClanInfo, UITabClan_Normal);

private:
	HDC				m_hdc;

	int				m_nSubjectWidth;
	int				m_nEllipseWidth;
protected:
	virtual void	_InitializeAtLoad( i3UIScene* pScene ) override;
	virtual void	_ClearAtUnload( i3UIScene* pScene ) override;

	void			_SetClanInfoNotice( void);	// 공지사항

	void			_SetEliteMember(INT32 idx, const CLAN_MEMBER_BASIC_INFO * pInfo);
	void			_UpdateEliteMember(void);

	void			_SetPrevMatchMemeberNick(i3::stack_wstring& out_wstr, const i3::wliteral_range& wNickRng);
	void			_UpdateClanNews(void);

	void			_RequestClanDetailInfo(void);
	
public:
	UITabClanInfo();
	~UITabClanInfo();

	virtual void	OnCreate(i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEntranceStart( void * pArg = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart( void) override;
	virtual void	OnLoadAllScenes() override;
};

#endif