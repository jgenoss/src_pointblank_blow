#pragma once

#include "GlobalInfoDef.h"

class GameLoginContext : public i3::shared_ginst<GameLoginContext>
{
public:	
	bool				IsFirstLogin() const		{ return m_bFirstLogin; }
	void				ResetFirstLogin()			{ m_bFirstLogin = false; }

	const GAME_LOGIN_INFO *	getLoginInfo() const	{ return &m_GameLoginInfo; }
	GAME_LOGIN_INFO *	setLoginInfo()				{ return &m_GameLoginInfo; }

	INT64				getUserUID(void) const { return m_GameLoginInfo.m_UserUID; }
	void				setUserUID(INT64 UID) { m_GameLoginInfo.SetUserUID(UID); }

	void				SetExtUID(const char *	_strExtUID) { m_GameLoginInfo.SetExtUID(_strExtUID); }
	const char *		GetExtUID() { return &m_GameLoginInfo.m_strExtUID[0]; }

	void				setWebLoginKey(const char* key);
	const char*			getWebLoginKey();

	const char *	getUserID( void ) const			{ return m_GameLoginInfo.m_UserId; }
	const char *	getUserPW( void ) const			{ return m_GameLoginInfo.m_UserPw; }
	const i3::rc_wstring&	getUserNick(void) const			{ return m_GameLoginInfo.m_wstrUserNick; }

	void		setUserID( const char * ID )		{ m_GameLoginInfo.SetUserID( ID); }
	void		setUserPW( const char * PW )		{ m_GameLoginInfo.SetUserPassword( PW); }
	void		setUserNick( const i3::rc_wstring& wstrNick)		{ m_GameLoginInfo.SetUserNick( wstrNick); }

	bool		IsWebLogin(void) const				{ return m_GameLoginInfo.m_bWebLogin;				}
	void		setWebLogin(bool bWebLogin)			{ m_GameLoginInfo.m_bWebLogin = bWebLogin;			}

	void		SetMacAddress(INT64* pMacAddress)	{ I3ASSERT( pMacAddress); m_GameLoginInfo.m_MacAddress = *pMacAddress; }
	const UINT64*	GetMacAddress(void) const		{ return &m_GameLoginInfo.m_MacAddress; }

	void		SetSessionKey(UINT8* pSessionKey)	{ m_GameLoginInfo.SetSessionKey( pSessionKey);	}
	const UINT8 *	GetSessionKey(void)	const		{ return &m_GameLoginInfo.m_SessionKey[0];		}

	void		SetSessionID(const char * pID)		{ m_GameLoginInfo.SetSessionID( pID); }
	const char *	GetSessionID(void)	const		{ return &m_GameLoginInfo.m_SessionID[0];		}

	void		SetLogInState(bool bTrue) { m_bIsLoginComplete = bTrue; }
	bool		IsLoginComplete(void)const { return m_bIsLoginComplete; }

	const char *	getBillingID(void) { return m_GameLoginInfo.m_BillingID; }
	void		setBillingID(const char* BillingID) { m_GameLoginInfo.SetBillingID(BillingID); }

	void		SetLinkMethod(UINT8	LinkMethod) { m_GameLoginInfo.SetLinkMethod(LinkMethod); }
	UINT8		GetLinkMethod() { return m_GameLoginInfo.m_LinkMethod; }

	bool		ParseCommandLineForLogin( const char * pszCmdLine);

	char*		GetID_ForRussiaWebLogin()			{	return m_szID_ForRussiaWebLogin;	}
	char*		GetPass_ForRussiaWebLogin()			{	return m_szPass_ForRussiaWebLogin;	}

#if defined (LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined(LOCALE_KOREA) || defined (LOCALE_VIETNAM)
	void	SetToken_ForGarena(char* pszToken);
#elif defined( LOCALE_JAPAN )
	void	SetArarioSessionKey(const char * pszSessionKey);
	void	SetArarioUSN(UINT32 SerialNum) { m_ui32ArarioUSN = SerialNum; }
	UINT32	GetArarioUSN(void) { return m_ui32ArarioUSN; }
#endif

	const std::string&	GetReportIP() const			{ return m_ReportIP; }
	UINT32				GetReportPort() const		{ return m_ReportPort; }
	UINT32				GetReportServerTime() const { return m_ReportServerTime; }
	void				SetReportFTPAddress( const std::string& ip, UINT16 port );
	void				SetReportFTPTime( UINT32 serverTime );

	void					SetIngameURLInfo(BANNER_URL_INFO info)		{ m_vecIngameURLInfo.push_back(info); }
	const BANNER_URL_INFO*	GetIngameURLInfo(int idx)					{ if (idx >= 0 && (size_t)idx < m_vecIngameURLInfo.size()) return &m_vecIngameURLInfo[idx]; 
																		  else return nullptr; }
	void					GetIngameURLInfo(i3::vector<BANNER_URL_INFO> &out, URL_TYPE type);
	void					ClearIngameURLInfo()						{ m_vecIngameURLInfo.clear(); }

	void				SetIngameURLVersion(UINT8 URLVersion) { m_ui8URLVersion = URLVersion; }
	UINT8				GetIngameURLVersion() { return m_ui8URLVersion; }

private:	
	
	void		SetID_AndPassForRussiaWebLogin(char* pszID, char* pszPass);
	void		SetID_AndPassForLatinWebLogin(const char* pszID, const char* pszPass,INT32 szAuthCode);
	
	//----------------------------------------------------------------------------------//
	//									중남미 웹로그인
	//----------------------------------------------------------------------------------//
	INT32				m_nAuthCode = 0;		// AuthCode

	GAME_LOGIN_INFO		m_GameLoginInfo;
	char				m_strWebLoginKey[WEB_LOGINKEY_SIZE] = { 0 };

	// 러시아 웹로그인
private:
	char				m_szID_ForRussiaWebLogin[MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN] = { 0 };							// Web 로그인 id
	char				m_szPass_ForRussiaWebLogin[MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN] = { 0 };						// Web 로그인 password


	// 가레나 로그인
#if defined (LOCALE_SINGMAL) || defined (LOCALE_PHILIPPINES) || defined(LOCALE_KOREA) || defined (LOCALE_VIETNAM)
	char			m_szToken_ForGarena[NET_ENCRYPTED_TOKEN_SIZE] = { 0 };
#elif defined(LOCALE_JAPAN)	
	char			m_szArarioSessionKey[NET_ENCRYPTED_TOKEN_SIZE] = { 0 };	// Session Key	( 로그인 인증용)
	UINT32			m_ui32ArarioUSN = 0;								// User Serial Number (캐시 충전 페이지용)
#endif

	std::string		m_ReportIP;
	UINT32			m_ReportPort = 0;
	UINT32			m_ReportServerTime = 0;

	bool			m_bFirstLogin = true;
	bool			m_bIsLoginComplete = false;

//배너 URL 정보
private:
	
	UINT8						m_ui8URLVersion = 0;
	i3::vector<BANNER_URL_INFO> m_vecIngameURLInfo;

};

