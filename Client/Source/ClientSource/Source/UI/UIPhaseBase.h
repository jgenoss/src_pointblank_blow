/**
 * \file	UI\UIPhaseBase.h
 *
 * Declares the user interface phase base class.
 */


#if !defined( __UI_PHASE_BASE_H__)
#define __UI_PHASE_BASE_H__

#include "UIBase.h"

typedef void (*CbINVITEPOPUP)(void * pThis, INT32 nParam);

enum UIPHASE;
class UIMainFrame;
class UIFramework;
class CUserLocationInfo;


static bool isWindowsXP()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = ::GetVersionEx ((OSVERSIONINFO *) &osvi);
	if( ! bOsVersionInfoEx )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return true;
	}

	if ( osvi.dwMajorVersion < 6 )
	{
		return true;
	}

	return false;
}

typedef UINT (CALLBACK* LPFNDLLFUNC) (GUID& rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
static CString getWritableFolder(void)
{ 
	CString strPath = _T("");
	TCHAR szPath[ MAX_PATH] = {0,};

	if( !isWindowsXP() )
	{ 
		PWSTR       path = nullptr;
		HINSTANCE   hInst;
		LPFNDLLFUNC pGetKnownFldPathFnPtr;

		// Vista 이상에서 사용자/AppData/Local 폴더 경로 {F1B32785-6FBA-4FCF-9D55-7B8E7F157091}, CSIDL_LOCAL_APPDATA 
		GUID folderid_localappdata = {0xF1B32785, 0x6FBA, 0x4FCF, {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}};

		hInst = LoadLibrary(_T("shell32.dll"));

		pGetKnownFldPathFnPtr = (LPFNDLLFUNC)GetProcAddress(hInst, "SHGetKnownFolderPath");

		if ( pGetKnownFldPathFnPtr && SUCCEEDED( pGetKnownFldPathFnPtr( folderid_localappdata, 0, nullptr, &path) ) )
		{

#ifndef _UNICODE
			i3String::WideCharToMultiByte( CP_ACP, 0, path, wcslen(path), szPath, MAX_PATH, nullptr, nullptr );
			strPath = szPath;
#else
			strPath = path;

#endif
			CoTaskMemFree( path );
		}

		FreeLibrary( hInst );
	}
	else
	{
		::SHGetSpecialFolderPath( NULL, szPath, CSIDL_APPDATA, TRUE );
		strPath = szPath;
	}

	return strPath;
}

class UIPhaseBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPhaseBase, UIBase);

protected:
	i3UIScene *		m_pScene;
public:
	bool			m_bMyPhase;	// Event를 보내는 시점부터 자신의 위치가 아닌 것으로 간주하여 서버에 Event를 보내지 못하도록 한다.

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPhaseBase();
	virtual ~UIPhaseBase();

	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void	OnUpdate_NoneRsc(REAL32 rDeltaSeconds) {}

	bool			isMyPhase(void)		{ return m_bMyPhase; }

	//******************************************************************************//
	//								Base Functions									//
	//******************************************************************************//
	virtual bool	OnKey_Escape( void) override;

	virtual void	OnSlideInOnce() override;
	virtual void	OnSlideOutOnce() override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	//******************************************************************************//
	//								Phase Functions									//
	//******************************************************************************//
	/** \brief Phase입장 Event를 GameContext에 보낸다.
		\desc 입장 Event를 보내도 Phase로 들어오지는 않는다.
			 OnEnter가 호출될시 Phase로 진입하도록 한다.	*/
	virtual void	Enter( void) {}

	/** \brief Phase입장 Event를 받았다 */
	virtual void	OnEnter( void) {}

	/** \brief Phase퇴장 Event를 GameContext에 보낸다*/
	virtual void	Leave( void) {}

	/** \brief Phase퇴장 Event를 받았다.*/
	virtual void	OnLeave( void) {}
	
	virtual void	OnNotifyPrevPhase(INT32  phase_type) {}    // 이전 상태가 필요없을수도 있기 때문에, 
																// 여기서는 비워두고, 파생클래스가 알아서.. (11.06.27.수빈)
	virtual void	OnGotoBackPhase( void) {}
	virtual void	OnModifyPhase_EndSubPhase(UIPHASE& inoutCurrentPhase) {}		// 디폴트는 아무것도 안함...
	/** \brief 아이템 인증 */
	virtual void	AuthStartSuccess(INT32 arg) {}

	/** \brief 종료버튼 */
	virtual void			ClickExit(void) {}

	//------------------------------------------------------------------------------//
	bool			IsRun( void);

	/** \brief UIMainFrame 포인터를 반환한다 */
	UIMainFrame *	GetMainFrame( void);

	/** \brief 베틀시작을 위해서 가상함수 정의합니다 */
	virtual void	JoinGame(void) {}

	//------------------------------------------------------------------------------//
	//									보급 상자									//
	//------------------------------------------------------------------------------//
protected:
	void			_ApplyMsgReadyBox( void);

	//------------------------------------------------------------------------------//
	//									초대하기									//
	//------------------------------------------------------------------------------//
	
private:
	void		ReceiveGameEvent_InvitedMessage( INT32 arg, const i3::user_data& UserData);
	void		ReceiveGameEvent_InvitedMessageClan(INT32, const i3::user_data&);
	void		ReceiveGameEvent_InvitedMessageNormalUser(INT32, const i3::user_data&);

public:

	bool			IsEnableInviteMsg(INT64 i64UID);

	void			OpenInviteMessage(void);
	void			OpenInviteMessage_NormalUser(void);
	void			OpenInvitemessage_Friend(void);
	void			OpenInviteMessage_Clan(void);
	
	virtual bool	OnInviteMove( void)	{ return false; }

	//------------------------------------------------------------------------------//
	//								Callback Functions								//
	//------------------------------------------------------------------------------//
	// 친구 초대 메시지 callback function
	static void		CbInviteMove( void * pThis, INT32 nParam);
	
	void			PopupInviteMessage( const CUserLocationInfo & UserLocInfo, const i3::rc_wstring& wstrNickName, CbINVITEPOPUP pCallBackFunc);

private:
	typedef void	(UIPhaseBase::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Change_Chara_Nick( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Error( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Disconnection( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_GameGuard_Error( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_HackToolDetected(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Inventory( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Inventory( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Auth_Item( INT32 arg, const i3::user_data&);

	void			ReceiveGameEvent_Enter_Profile( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Profile(INT32 arg, const i3::user_data&);

	void			ReceiveGameEvent_Enter_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Shop( INT32 arg, const i3::user_data& UserData);
	void			ReceiveGameEvent_Leave_Shop( INT32 arg, const i3::user_data& UserData);
	void			ReceiveGameEvent_Delete_NoteAsk( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Note( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Insert_Block( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Accept_Friend( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Delete_Friend( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Invite_Clan( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Gacha( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Gacha( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Clan( INT32 arg, const i3::user_data & userData);
	void			ReceiveGameEvent_Leave_Clan( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_View_Other_User_Item_Info( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_ShutDown_Time( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_GiftShop( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_GiftShop( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Fail_Enter_GiftShop(INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_GM_User_Exit( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Enter_Temp( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Leave_Temp( INT32 arg, const i3::user_data&);

	//클랜 채널내의 제명 이벤트 모든 uiphase에서 작동되어야 한다.
	void			ReceiveGameEvent_Clan_Deprotation(INT32 arg, const i3::user_data&);

protected:
	/** \brief 자리비움 검사 
		\desc 일정 시간동안 입력이 없으면 퇴장시킨다. */
	void			ReceiveGameEvent_DriveAway( INT32 arg, const i3::user_data&);
	
	
		

public:
	// GameEvent를 처리한다.
	//GetMainFrame()->GameEventTransferToSubPhase(event,arg); 를 호출 하지 마라..
	//맨 아래에서 호출 한다.
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData);

};

#endif
