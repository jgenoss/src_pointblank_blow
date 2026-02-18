#ifndef _PhaseLobby_dmy_h
#define _PhaseLobby_dmy_h

#include "ReadyPhaseBase.h"
#include "StageDef.h"
#include "GameChat_Body.h"
#include "GuiButtonEffector.h"

class CMessageBox;

class CPhaseLobbyDmy: public CReadyPhaseBase
{
	I3_CLASS_DEFINE(CPhaseLobbyDmy);
public:
	CPhaseLobbyDmy();
	virtual ~CPhaseLobbyDmy();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnPreGuiNotify(void);
	virtual void OnUpdate(REAL32 rDeltaSeconds);	
	virtual void OnWheelUpdate(INT32 scrollby);	
	virtual void SetInputDisable(BOOL bDisable);
	virtual BOOL GetInputDisable(void);

	virtual void OnEntranceStart(void);
	virtual BOOL OnEntrancing(RT_REAL32 rDeltaSeconds);
	virtual void OnEntranceEnd(void);
	virtual void OnExitStart(void);
	virtual BOOL OnExiting(RT_REAL32 rDeltaSeconds);
	virtual void OnExitEnd(void);	

	virtual void OnSlideOut(BOOL bRight);
	virtual void OnSlideIn(BOOL bRight);
	
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	virtual BOOL OnSliding(RT_REAL32 rDeltaSeconds);

protected:
	i3UIScene	*	m_pUIScene;

	//로비에서 방만들기에 이용되는 변수.
	UINT32	m_nStageID;
	UINT8	m_SubType;
	UINT8	m_LockWeapon;
	UINT8	m_LockInterEnter;
	UINT8	m_LockObserver;
	UINT8	m_LockTeamBalance;
	i3List	m_MapList;
	I3COLOR	m_MapColor;
};

#endif //_PhaseLobby_h
