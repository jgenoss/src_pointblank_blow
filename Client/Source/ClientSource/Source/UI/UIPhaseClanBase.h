#if !defined( __UI_PHASE_CLAN_BASE_H__)
#define __UI_PHASE_CLAN_BASE_H__

#include "UIPopupBase.h"

class ClanProcessGameEvent
{
public:
	ClanProcessGameEvent();

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

private:
	typedef void	(ClanProcessGameEvent::*ClanEventFunc)(UIMainFrame*, INT32);
	i3::unordered_map< GAME_EVENT, ClanEventFunc >	m_eventMap;

	void			RegisterFunc( GAME_EVENT evt, ClanEventFunc f);

	void			ReceiveGameEvent_JoinCheck_Authority( UIMainFrame* pFrame, INT32 arg);
	void			ReceiveGameEvent_Deprotation(UIMainFrame* pFrame, INT32 arg);			//클랜페이지상에서 제명당했을때 처리
	void			ReceiveGameEvent_Dissolution( UIMainFrame* pFrame, INT32 arg);
	void			ReceiveGameEvent_Secession( UIMainFrame* pFrame, INT32 arg);
	void			ReceiveGameEvent_CommisionMaster( UIMainFrame* pFrame, INT32 arg);
	void			ReceiveGameEvent_Request_Cancel( UIMainFrame* pFrame, INT32 arg);
	void			ReceiveGameEvent_Invite_Accept( UIMainFrame* pFrame, INT32 arg);
};

class UIPhaseClanBase : public UIPopupBase, public ClanProcessGameEvent
{
	I3_ABSTRACT_CLASS_DEFINE( UIPhaseClanBase, UIPopupBase);

protected:

	void			_SetClanExpRate(i3::wstring& wstrOut, INT32 & i32OutRate , UINT32 ui32Exp);
	void			_SetClanTotalRecord(i3::wstring& OutRecord, i3::wstring& OutRate, CLAN_DETAIL_INFO * pInfo);
	void			_SetClanSeasonRecord(const CLAN_RECORD & info, i3::rc_wstring & out_match, i3::rc_wstring & out_win, i3::rc_wstring & out_lose);
	void			_SetClanMark(i3UIImageBoxEx * pImgBox, UINT32 ui32ClanMark);

	bool			_CheckClanRequsetEnable(void);
public:
	UIPhaseClanBase();
	virtual ~UIPhaseClanBase();

	virtual bool	OnExitStart() override;

	virtual void	OnExitEnd(void) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnKey_Escape( void) override;
	virtual void	OnEscapeKey( void) override;

	virtual void	OnGotoBackPhase( void);

	bool			isVisitUser( void) { return g_pFramework->GetClanContext()->IsClanVisit(); }

	// CallBack Function
	static void		cbCancelRequest(void* pThis, INT32 nParam);
};

#endif
