#if !defined( __STAGE_LOGIN_H)
#define __STAGE_LOGIN_H

#include "StageGUI.h"

enum GUI_ID_LOGIN_TYPE
{
	GUI_ID_LOGIN_BUTTON_JOIN = GUI_ID_OFFSET_LOGIN,
	GUI_ID_LOGIN_BUTTON_LOGIN,
	GUI_ID_REALNAME_BUTTON_CONFIRM,
	GUI_ID_REALNAME_LICENSE_CONFIRM,
	MAX_GUI_ID_LOGIN
};

enum STATE
{
	STATE_1
};

class CMessageBox;

class CStageLogin : public CStageGUI
{
	I3_CLASS_DEFINE( CStageLogin, CStageGUI);

protected:
	i3GuiStatic *			m_pStaticBG;
	i3GuiStatic *			m_pStaticBox;
	i3GuiStatic *			m_pStaticBGAccount;
	i3GuiStatic *			m_pStaticCarret;
	i3GuiEditBox *			m_pEditBoxID;
	i3GuiEditBox *			m_pEditBoxPW;
	i3GuiCheckBox * 		m_pCheckBox;
	i3GuiButton *			m_pButtonJoin;
	i3GuiEditBox*			m_pButtonJoinText;
	i3GuiButton *			m_pButtonLogin;
	i3GuiEditBox *			m_pButtonLoginText;

	i3GuiEditBox *			m_pFocusedControl;

	REAL32					m_rConnectWaitTimeStamp;
	BOOL					m_bConnectStart;

	CMessageBox*			m_pmbLoginWait;
	BOOL					m_LoginError;

	INT32					m_LogInConnectFailedCount;

	REAL32					m_GameStartTime;	// 게임 실행 후 로그인화면이 떴을때의 시간.
	BOOL					m_ForceExit;		// 일정시간 이상 로그인하지 않아서 게임을 강제종료하는지.
	BOOL					m_RNFlag;
	//*2011-03-07 하동익 중국 실명제 관련 UI 변수 추가

#if defined(LOCALE_CHINA)
	CMessageBox*			m_pmbRNWait;

	i3GuiStatic *			m_pStaticRNBG;
	i3GuiEditBox *			m_pStaticSRNName;
	i3GuiEditBox *			m_pStaticSRNSnum;
	i3GuiEditBox *			m_pStaticSRNEmail;
	i3GuiEditBox *			m_pStaticRNName;
	i3GuiEditBox *			m_pStaticRNSnum;
	i3GuiEditBox *			m_pStaticRNEmail;
	i3GuiEditBox *			m_pStaticRNNotice;
	i3GuiButton *			m_pButtonRNConfirm;

	i3GuiStatic *			m_pUserLicenseWindow;
	i3GuiStatic *			m_pUserLicenseScrollFrame;
	i3GuiEditBox *			m_pUserLicenseText;
	i3GuiList *				m_pUserLicenseList;
	i3GuiButton *			m_pUserLicenseConfirm;
	char					NoticeText[1024];
	INT32					m_iLineCnt;
	BOOL					m_LoginWindowFlag;
	
public:
	void					LoadUserCheck( char* pszPath);
	virtual	void			OnWheelUpdate(INT32 scrollby);
	void					OnLicenseListScroll();

#endif

public:
	CStageLogin(void);
	virtual ~CStageLogin(void);

	void					LinkControlByName();
	void					Init();

	BOOL					SaveID( char* pszPath);
	void					LoadID( char* pszPath);

	void					ProcessGameContext( REAL32 rDeltaSeconds);
	void					ConnectLogInServer();
	
protected:
	virtual void			OnCreate();
	virtual BOOL			OnQueryLoad( INT32 numStage);	
	virtual void			OnUpdate( REAL32 rDeltaSeconds);
	virtual BOOL			OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	
	virtual void			OnControlNotify(I3GUI_CONTROL_NOTIFY* pNotify);

private:
	void					_WaitConnectMessageBox(REAL32 rDeltaSeconds);
	void					_ResetWaitConnect(void);
	void					_CheckGameRunTimeOut(void);


public:	// Notify Handler
	void					OnJoinButtonClick(void);
	void					OnLoginButtonClick(void);
	void					SetAnimation(void);
#if defined(LOCALE_CHINA)
	//2011-03-07 하동익 중국 실명제 관련 UI 변수 추가
	void					OnRealnameButtonClick(void);
	void					OnRealnameLicenseButtonClick(void);
#endif

public: // Error Handler
	void					EhNonID(void);
	void					EhNonPW(void);
	void					EhInvalidAccount(void);
	void					EhInvalidID(void);
	void					EhInvalidPassword(void);
	void					EhInvalidName(void);

	// 북미 Error Handler 추가 (2011.06.10 양승천)
	void					EhInvalidAccount(const char* Msg);
	void					EhInvalidCountry(void);
	
	void					EhLogouting(void);
	void					EhAlreadyLogin(void);
	void					EhTimeOut1(void);
	void					EhTimeOut2(void);
	void					EhBlockAccount(INT32 Arg = 0);
	void					EhUnknownLogin(INT32 Event, INT32 Arg);
	void					EhInventoryFail(void);
	void					EhWaitDisconnect(void);
	void					EhFailConnect(void);
	void					EhStageError(INT32 Event,INT32 Arg);
	void					EhBlockGame(INT32 Arg);
	void					EhBlockInner(INT32 Arg);
	void					EhBlockOuter(INT32 Arg);

	void					EhDBBufferFull(void);
	void					EhDeletedAccount(void);
	void					EhEmailAuthError(void);
	void					EhBlockIP(void);

	void 					FocusToControl(void)				{ if(m_pFocusedControl) m_pFocusedControl->SetFocus(TRUE);					}


	ErrorMsgHandler			GetErrorHandler();

public:
	static void				CbFailSaveID(void* pThis,INT32 nParam);
	static void				CbAlreadyLogin(void* pThis,INT32 nParam);
	static void				CbPopupOK(void* pThis,INT32 nParam);

	static BOOL				GetEnvSessKey(UINT8* pSessKey);
	static BOOL			    GetEnvMacAddr(INT64* pMacAddr);
	static BOOL				GetEnvServerAddr(char* strServerAddr, const size_t sizeServerAddrStrMax);
#if defined(LOCALE_CHINA)
	virtual void			OnRevive( i3RenderContext * pCtx);
#endif


protected:
	static void				MsgBoxCallBack_LoginFailWithBadID(void* pThis,INT32 nParam);

};

#endif // __STAGE_LOGIN_H
