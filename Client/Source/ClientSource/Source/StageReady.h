#ifndef _StageReady_h
#define _StageReady_h

#include "GuiDef.h"
#include "StageDef.h"
#include "GameCharaDef.h"
#include "ReadyBg.h"
#include "GuiButtonEffector.h"
#include "ToolTip.h"

//Room Master GameStart Timer After All User Ready
#define		USE_ROOM_MASTER_KICK



class CPhaseBase;
class CStepBase;
class CMenuBar;
class CAwayChecker;
class CPopupBase;
class CReadyBg;
class CMessageBox;
class CContextMenu;
class CSpecialCharInput;
class CBgChallenge;


class CStageReady: public i3Stage
{
	I3_CLASS_DEFINE(CStageReady);
public:
	CStageReady();
	virtual ~CStageReady();

	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual BOOL OnQueryLoad(INT32 numStage);
	virtual void OnCreate(void);
	virtual BOOL OnFinish(void);
	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);	
	virtual void OnChangeVideo( i3VideoInfo * pInfo);
	virtual void OnRevive( i3RenderContext * pCtx);

	void MovePhase(PhaseType phaseType);
	void ReturnClanBattleLobby(void);
	void SetMenuBarInputDisable(BOOL bEnable);
	void SetMenuClanInputDisable(BOOL bEnable);
	void StartAlarmCommunity(void);
	void StartAlarmNote(void);
	void ChangeStep(StepType stepType);
	void TogglePopup(PopupType popupType, void * pArg1 = NULL, void * pArg2 = NULL);
	void SetTooltip(UINT32 idxTooltip);
	BOOL UpdateCharacter(ViewType viewType,BOOL bWeaponLimit = TRUE, VIEW_CHARA_INFO * pNewChara = NULL);

	// Character Ceremony 추가 2008.07.03 dfly79
	void	PlayCeremonyCharacter();
	void	MoveCamera(CameraLook lookType);
	CPhaseBase *	GetCurrentPhase(void)				{return m_pPhase[m_currentPhase];}
	CStepBase *		GetCurrentStep(void)				{return m_pStep[m_currentStep];}
	char *			getChannelSlotText(INT32 index)		{return m_szServerSlotText[index];}
	void StartPhase(PhaseType MovePhase);

	static void SetLastPhaseLobby(void);

	BOOL IsSlidingInventory(void);
	
	VIEW_CHARA_INFO * GetViewCharaInfo(void);

	void GetCurrentSeverInfo(char * pszString, const size_t sizeStringMax);

	void EnterInventory(void);
	void LeaveInventory(void);
	BOOL IsEnterInventory(void);
	void AuthStartSuccess(INT32 arg);

	void EnterShop(void);
	void LeaveShop(void);
	BOOL IsEnterShop(void);
	BOOL IsEnterClanMember(void);
	BOOL IsEnterClanNonMember(void);

	void EnterInfo(void);
	void LeaveInfo(void);
	BOOL IsEnterInfo(void);

	void EnterClanNonMember(void);
	void LeaveClanNonMember(void);

	void EnterClanMember(void);
	void LeaveClanMember(void);

	void ExclusedEnter(StepType stepType);
	void EnterDelayedEventStep(void);

	void OnInviteMove(void);
	void OnInviteMoveClan(void);

	void TransferrToPopupGameEvent( INT32 event, INT32 Arg);
	BOOL IsEnablePopup( PopupType type);

	void TransferrToStepGameEvent( INT32 event, INT32 Arg);

	PhaseType	GetCurrentPhaseType(void)		{return m_currentPhase;}	
	StepType	GetCurrentStepType(void)		{return m_currentStep;}
	StepType	GetOldStep(void)				{return m_oldStep;}
	CReadyBg *	GetReadyBg(void)				{return m_pReadyBg;}
	CStepBase *	GetStep(StepType Type)			{return m_pStep[Type];}
	i3GuiRoot * GetGuiRoot(void)				{return m_pGuiRoot;}

	// ContextMenu
	void ContextMenuReset(void);
	void ContextMenuOpen(i3EventReceiver* observer,INT32 requestId);
	void ContextMenuAddCommand(ContextMenuCommand command);

#if defined(CHANGE_ADDCOMMAND_METHOD)
	void CopyContextMenuList(INT32 MenuList[64]);
#endif

	void SetPushedStateButton(INT32 menuButtonType,BOOL bPushed);

	BOOL IsPopup_Option(void);
	void Popup_OptionClosed(void);

	//Room Master GameStart Timer After All User Ready
	void InitRoomMasterKick();
	void CreateRoomMasterKick();
	void RemoveRoomMasterKick();
	void PauseRoomMasterKick();	
	void CheckRoomMasterKick();
	void StopRoomMasterKick();
	void ResetRoomMasterKick();
	#ifdef USE_ROOM_MASTER_KICK
	void Set_bBeHostMessageOK(BOOL bFlag) { m_bBeHostMessageOK = bFlag; }
	void Set_bPS_PLAYING(BOOL bFlag) { m_bPS_PLAYING = bFlag; }		
	void Set_bAllUserReady(BOOL bFlag) { m_bAllUserReady = bFlag; }
	void Set_bRoomMsterAwayCountDown(BOOL bFlag) { m_bRoomMsterAwayCountDown = bFlag; }
	
	void SetEffectButton(void);
	
	
	BOOL Get_bBeHostMessageOK() { return m_bBeHostMessageOK; }
	BOOL Get_bPS_PLAYING() { return m_bPS_PLAYING; }	
	BOOL Get_bAllUserReady() { return m_bAllUserReady; }
	BOOL Get_bRoomMsterAwayCountDown() { return m_bRoomMsterAwayCountDown; }
	BOOL Get_bGameStart() { return m_bGameStart; }

	void SetEffectBegin(void) { BeginEffect = TRUE; }
	void SetEffectButtonBegin(void) {	BeginEffect = TRUE; 	m_ActiveTime = 0.0f;   }
	void SetEffectButtonStop(void)  {	BeginEffect = FALSE; 	m_ActiveTime = 0.0f;   }
	BOOL GetButtonEffect() { return Button_Effect_State; }

	INT32 GetRoomMasterAwayCheckerTime() { return m_RoomMasterAwayCheckerTime; }
	#endif

	void SetExitMsgBoxOffset(INT32 x, INT32 y);
	void UpdateEffectTime(RT_REAL32);

	void EnterNickSpace();
	void OutroNickSpace();

protected:
	char	m_szServerSlotText[SERVERSELECT_MAINTYPE_COUNT][256];
		
private:
    void _GuiNotifyPump(void);
	void _GameEventPump(void);
	void _LinkControl(void);
	void _InitControl(void);
	void _CheckAwayInput(void);
	void _ProcessWheel(void);
	void _Rec_BuildScene( i3GuiObjBase * pObj);

	void _BackgroupEnable(void);

private:
	i3GuiRoot* m_pGuiRoot;
	CBgChallenge* m_pChallenge;
	CPhaseBase* m_pPhase[MAX_PHASE];
	CMenuBar* m_pMenuBar;
	CContextMenu* m_pContextMenu;
	CStepBase* m_pStep[MAX_STEP];
	CPopupBase* m_pPopup[MAX_POPUP];
	PhaseType m_currentPhase;
//	List	m_PhaseQueue;

	PhaseState m_phaseState;
	StepType m_currentStep;
	StepType m_oldStep;

	static PhaseType m_lastPhase;

	CReadyBg* m_pReadyBg;
	CSpecialCharInput * m_pSpCharInput;
	CToolTip		m_ToolTip;

	CameraLook	m_currentLook;

	REAL32 m_rWheelDelta;

	BOOL m_bInventory;
	BOOL m_bShop;
	BOOL m_bInfo;
	BOOL m_bClanMember;
	BOOL m_bClanNonMember;
	StepType m_ExclusedEnterEventStep;

	BOOL m_bInClanBattleLobby;

#if defined	USE_AWAY_INPUT
	CAwayChecker* m_pAwayChecker;
	CMessageBox* m_pAwayMessageBox;
	BOOL m_bProcessLeaveRoom;
	StepType m_oldAwayStep;

	#ifdef USE_ROOM_MASTER_KICK
	//Room Master GameStart Timer After All User Ready
	BOOL			m_bBeHostMessageOK;
	BOOL			m_bPS_PLAYING;
	BOOL			m_bAllUserReady;
	BOOL			m_bRoomMsterAwayCountDown;
	BOOL			m_bGameStart;
	BOOL			m_bSetRoomMasterKick;	
	INT32			m_RoomMasterAwayCheckerTime;

	REAL32			m_ActiveTime;
	BOOL			Button_Effect_State;

	BOOL			BeginEffect;

	CAwayChecker*	m_pRoomMasterAwayChecker;
	i3SoundPlayInfo* m_pRoomMasterCountdownSound;
	#endif
#endif

#if defined USE_LUA_UI
	virtual void	RegisterLuaFunction( lua_State * pState);
#endif

	// 치팅 방어
public:
	void CheckCheating();

	// 방 시작 카운트 처리
private:
	INT32 m_iRemainCount;
	REAL32 m_rRemainTime;
public:
	void SetRemainCount(INT32 iCount);
	void DecreaseRemainCount() { m_iRemainCount -= 1; SetRemainCount(m_iRemainCount); }
	void Set_bGameStart(BOOL bFlag);
	void StartCount(INT32 iCount);
};

#endif //_StageReady_h
