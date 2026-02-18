#ifndef _PhaseLogin_h
#define _PhaseLogin_h

#include "PhaseBase.h"

class CMessageBox;

class CPhaseLogin: public CPhaseBase
{
	I3_CLASS_DEFINE(CPhaseLogin);
public:
	CPhaseLogin();
	virtual ~CPhaseLogin();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual BOOL OnLinkControl(i3GuiControl* pControl);
	virtual void OnInitControl(void);
	virtual void OnUpdate(REAL32 rDeltaSeconds);
	virtual void OnEntranceStart(void);

	BOOL SaveID(char* pszPath);
	void LoadID(char* pszPath);

	void OnJoinButtonClick(void);
	void OnLoginButtonClick(void);

	void EhNonID(void);
	void EhNonPW(void);
	void EhInvalidAccount(void);
	void EhAlreadyLogin(void);	
	void EhTimeOut1(void);
	void EhTimeOut2(void);
	void EhBlockAccount(void);
	void EhUnknownLogin(void);
	void EhWaitDisconnect(void);
	void EhFailConnect(void);
	void EhFailChannel(void);
	void EhStageError(INT32 Event,INT32 Arg);
	void EhBlockGame(INT32 Arg);
	void EhBlockInner(INT32 Arg);
	void EhBlockOuter(INT32 Arg);

private:
	void _WaitConnectMessageBox(REAL32 rDeltaSeconds);
	void _ResetWaitConnect(void);	

public:
	static void	CbFailSaveID(void* pThis,INT32 nParam);
	static void CbAlreadyLogin(void* pThis,INT32 nParam);

	static BOOL GetEnvSessKey(UINT8* pSessKey);
	static BOOL GetEnvMacAddr(INT64* pMacAddr);
	static BOOL GetEnvServerAddr(char* strServerAddr);
	static INT8 HexToInt(char chHex); // [0-9a-fA-F]

private:
	i3GuiStatic* m_pStaticBg;
	i3GuiStatic* m_pStaticCarret;
	i3GuiButton* m_pButtonJoin;
	i3GuiButton* m_pButtonLogin;
	i3GuiEditBox* m_pEditBoxId;
	i3GuiEditBox* m_pEditBoxPw;
	i3GuiCheckBox* m_pCheckBox;

	REAL32 m_rConnectWaitTimeStamp;
	BOOL m_bConnectStart;

	CMessageBox* m_pmbLoginWait;
};

#endif //_PhaseLogin_h
