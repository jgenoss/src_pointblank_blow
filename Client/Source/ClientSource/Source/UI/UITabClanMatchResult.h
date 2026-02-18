#if !defined( __UI_TAB_CLAN_MATCH_RESILT_H__)
#define __UI_TAB_CLAN_MATCH_RESILT_H__

#include "UITabClan.h"

#define CLAN_MATCH_RESULT_SLOT_COUNT	3

class UITabClanMatchResult : public UITabClan_Normal
{
	I3_CLASS_DEFINE( UITabClanMatchResult, UITabClan_Normal);
private:
	i3UIScrollBar *	m_pMRListScroll;	// Match Result Scroll

	HDC				m_hdc;

	int				m_nSubjectWidth;
	int				m_nEllipseWidth;
	i3::vector<CLAN_WAR_RESULT> m_MatchList;
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	void			_InitUI(void);
	void			_InitScrollBar(void);

	void			_UpdateMatchResultList(void);
	void			_SetMatchResultInfo(INT32 i32Slot, const CLAN_WAR_RESULT * pInfo);
	void			_SetMatchResultStageInfo(INT32 i32Slot, const CLAN_WAR_RESULT * pInfo);

public:
	UITabClanMatchResult();
	~UITabClanMatchResult();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1  = nullptr, void * pArg2  = nullptr ) override;
	virtual void	OnEntranceEnd(void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	void			MatchResultListScroll(void);
	void			MatchResultListWheel(INT32 zDelta);

private:
	typedef void		(UITabClanMatchResult::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void				RegisterGameEventFunc(GAME_EVENT evt, call_func f);

	void				ReceiveGameEvent_MatchResult(INT32 arg, const i3::user_data& data);
};

#endif