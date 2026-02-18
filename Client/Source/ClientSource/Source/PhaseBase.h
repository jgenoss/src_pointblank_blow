#ifndef _PhaseBase_h
#define _PhaseBase_h

#include "GuiNotifyReceiver.h"
#include "StageDef.h"
#include "GameChat_Body.h"
#include "i3Base/itl/vector_set_fwd.h"

class CUserLocationInfo;

class CPhaseBase: public CGuiNotifyReceiver
{
	I3_ABSTRACT_CLASS_DEFINE(CPhaseBase, CGuiNotifyReceiver);
public:
	CPhaseBase();
	virtual ~CPhaseBase();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) = 0;
	virtual BOOL OnLinkControl(const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl) = 0;
	virtual void OnInitControl(void);

	virtual void OnGameEvent(INT32 event,INT32 arg);

	virtual void OnEntranceStart(void);
	virtual BOOL OnEntrancing(RT_REAL32 rDeltaSeconds);
	virtual void OnEntranceEnd(void);
	virtual void OnExitStart(void);
	virtual BOOL OnExiting(RT_REAL32 rDeltaSeconds);
	virtual void OnExitEnd(void);
	
	virtual void OnSlideOut(BOOL bRight);
	virtual void OnSlideIn(BOOL bRight);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual void OnChangeVideo( i3VideoInfo * pInfo);

	void DriveAway(INT32 nReason);
	virtual void OnExitKey(void);

	CGameChatBody *	GetChatBody( void)			{ return m_pChatBody;		}

public:
	void OnInvitedMessage(void);
	static void CbInviteFriend(void* pThis,INT32 nParam);

	void OnInvitedMessageClan(void);
	static void CbInviteClan(void* pThis,INT32 nParam);

	typedef void (*InvitePopupCallback)(void * pThis, INT32 nParam);
	void	PopupInviteMessage(const CUserLocationInfo & UserLocInfo, const char * Nickname, InvitePopupCallback pCallBackFunc);


	virtual	BOOL OnInviteMove(void)		{return FALSE;}

protected:

	// Ã¤ÆÃ
	CGameChatBody * m_pChatBody;
	

};

#endif //_PhaseBase_h
