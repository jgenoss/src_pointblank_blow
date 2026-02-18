#ifndef _MenuBar_h
#define _MenuBar_h

#include "GuiNotifyReceiver.h"
#include "GuiButtonEffector.h"

enum FlowState
{
	FS_INIT = 0,
	FS_READ_LETTER,
	FS_FLOW,
	FS_MAX
};

class CMessageBox;

class CMenuBar: public CGuiNotifyReceiver
{
	I3_CLASS_DEFINE(CMenuBar);
public:
	CMenuBar();
	virtual ~CMenuBar();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void OnClan(void);
	void OnProfile(void);
	void OnInventory(void);
	void OnShop(void);
	void OnCommunity(void);
	void OnOption(void);
	void OnExit(void);

	void SetInputDisable(BOOL bEnable);
	void SetClanInputDisable(BOOL bEnable);

	void SetServerMode(void);
	void SetReadyMode(void);

	void StartAlarmCommunity(void);
	void EndAlarmCommunity(void);

	void SetPushedStateButton(INT32 menuButtonType,BOOL bPushed);
	void SetExitMsgBoxOffset(INT32 x, INT32 y);

	void UpdateButton();

	i3GuiButton* GetEffectButton();
	
	static void SetFlowAnnounce(const char* strAnnounce);
	static void SetFlowAnnounce2(const char* strAnnounce);

	void EnterNickSpace();
	void OutroNickSpace();

private:	
	void _UpdateUserInfo(void);
	void _ShowRankMark(INT32 nRank);
	INT32 _GetExpMax(INT32 nRank);
	void _ShowDotRate(INT32 nExpRate);
	
	void _InitFlowAnnounce(void);
	void _DestroyFlowAnnounce(void);
	void _UpdateFlowAnnounce(REAL32 rDeltaSeconds);

	void _UpdateAlarm(REAL32 rDeltaSeconds);

	void _UpdateRoomMasterKickAlarm(void);

private:
	i3GuiStatic* m_pMenuBar;
	i3GuiStatic* m_pInfoBar;

	i3GuiButton* m_pClan;
	i3GuiButton* m_pProfile;
	i3GuiButton* m_pInventory;
	i3GuiButton* m_pShop;
	i3GuiButton* m_pCommunity;
	i3GuiButton* m_pOption;
	i3GuiButton* m_pExit;

	i3GuiEditBox* m_pNickname;
	i3GuiEditBox* m_pExp;
	i3GuiEditBox* m_pPinoKey;
	i3GuiEditBox* m_pPinoValue;
	i3GuiEditBox* m_pNcCoinKey;
	i3GuiEditBox* m_pNcCoinValue;
	
	i3GuiStatic* m_pRank;
	i3GuiStatic* m_pExpBar;
	i3GuiStatic* m_pExpGage;
	i3GuiStatic* m_pESport;

	i3GuiEditBox* m_pExpText;
	
	i3GuiEditBox* m_pAnnounce;	


	// Èå¸£´Â °øÁö
	static char m_strAnnounceBuffer[MAX_STRING_COUNT];
	static INT32 m_idxAnnounceLetter;
	INT32 m_nAnnounceWidth;
	INT32 m_nMoveOffset;
	REAL32 m_rCurrentOffset;
	i3TextNodeDX2* m_pCalculateText;
	FlowState m_flowState;
	INT32 m_nMaxBlankLength;
	INT32 m_nCurBlankLength;

	BOOL m_bAlarmCommunity;
	REAL32 m_rAlarmTerm;
	BOOL m_bAlarmFlag;
	BOOL m_FFlow;

	CMessageBox*	m_pMessageBox;
	BOOL			m_bRoomMsterKickAlarmFlag;
	
	INT32			m_nCurrentCountDown;	
	POINT32			m_ExitMsgBoxOffset;
	REAL32			m_bFlowTime;


	bool m_bNickSpace;

	//FCM
	static char m_strAnnounceBuffer2[MAX_STRING_COUNT];
	static BOOL	m_bFCMFlag;	
	static BOOL m_bFlowFlag;
};

#endif //_MenuBar_h
