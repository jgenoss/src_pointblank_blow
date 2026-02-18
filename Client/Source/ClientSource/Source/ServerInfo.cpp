#include "pch.h"
#include "ServerInfo.h"
#include "ServerIPList.h"


#define SERVER_PORT_TCP			39190
#define SERVER_PORT_UDP			39199
#define	MAX_SERVER_USER			3000	// 임시임. 서버에서 보내주는것으로 수정해야함.


I3_CLASS_INSTANCE( CServerInfo);//, i3ElementBase);

CServerInfo::CServerInfo()
: m_Number(0)
, m_ServerIP(0)
, m_ServerPort(0)
, m_Type(S_CHANNEL_TYPE_UNDEFINED)
, m_UserCount(0)
, m_MaxUserCount(0)
, m_Active(false)
{
	i3mem::FillZero(m_ServerIPStr, sizeof(m_ServerIPStr));
}


void CServerInfo::Set( UINT32 Number, const GAME_SERVER_INFO & ServerInfo )
{
	m_Number		= Number;
	m_ServerIP		= ServerInfo._OutIP;
	m_ServerPort	= ServerInfo._OutPort;//SERVER_PORT_TCP;
	m_Type			= static_cast<SChannelType>(ServerInfo._AllType);
	m_UserCount		= static_cast<UINT32>(ServerInfo._Arg0);
	m_Active		= ServerInfo._IsActive != 0;

	in_addr	IP;
	IP.S_un.S_addr	= ServerInfo._OutIP;
	i3::safe_string_copy( m_ServerIPStr, inet_ntoa(IP), 64 );
}



//////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE( CServerInfoContext);//, i3ElementBase);

CServerInfoContext::CServerInfoContext()
{
	m_IsUseServerSelect		= false;
	m_pServerIPManager		= nullptr;
	m_GameServerIndexByType	= -1;
	m_GameServerType		= S_CHANNEL_TYPE_UNDEFINED;
	m_CurChannelNumber		= INVALID_INDEX;
}


CServerInfoContext::~CServerInfoContext()
{
	I3_SAFE_RELEASE(m_pServerIPManager);
	Clear();
}

bool CServerInfoContext::_IsDottedAddressIP( const char* pAddr ) const
{
	char buffer[64] = {0};
	const char* pAddress = pAddr;

	while( *pAddress != '\0' )
	{
		if( *pAddress != '.' ) ::strncat( buffer, pAddress, 1 );

		if( *(pAddress+1) == '.' || *(pAddress+1) == '\0' )
		{
			if( ::_ttoi( buffer ) >= 0xff ) return false;
			else i3::safe_string_copy( buffer, "", 64 );
		}

		pAddress++;
	}

	if( i3::generic_string_size(pAddr) < 7 ) return false;

	if( INADDR_NONE == ::inet_addr(pAddr) ) return false;

	return true;
}

const char*	CServerInfoContext::_ConvertDomainName2IP( const char* pAddr ) const
{
	struct hostent* pRemoteHost = nullptr;

	pRemoteHost = gethostbyname( pAddr );
	if( pRemoteHost == nullptr )
	{
		I3PRINTLOG(I3LOG_DEFALUT, " ## Fail to Convert DomainName to Dotted Address IP!! ");
		I3PRINTLOG(I3LOG_DEFALUT, " ## Invalid Domain or IP : %s", pAddr );

		I3PRINTLOG(I3LOG_WARN, "Invalid DomainName or IP!!");

		return nullptr;
	}
	else
	{
		const char* pIP = ::inet_ntoa( *(in_addr*)*pRemoteHost->h_addr_list );

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)

		I3PRINTLOG(I3LOG_DEFALUT, " ## official name: %s", pRemoteHost->h_name);
		I3PRINTLOG(I3LOG_DEFALUT, " ## Alternate Names: %s", pRemoteHost->h_aliases);
		switch( pRemoteHost->h_addrtype )
		{
		case AF_INET:
			I3PRINTLOG(I3LOG_DEFALUT, " ## address type: AF_INET ");
			break;
		case AF_INET6:
			I3PRINTLOG(I3LOG_DEFALUT, " ## address type: AF_INET6 ");
			break;
		case AF_NETBIOS:
			I3PRINTLOG(I3LOG_DEFALUT, " ## address type: AF_NETBIOS ");
			break;
		}

		I3PRINTLOG(I3LOG_DEFALUT, " ## IP Address : %s", pIP);
		 
#endif

		return pIP;
	}
}

bool	CServerInfoContext::Initialize()
{
	m_pServerIPManager = CServerIPList::new_object();
	return m_pServerIPManager->Load();
}

INT32	CServerInfoContext::GetServerTypeCount(SChannelType SType) const
{
	if( !(SType > S_CHANNEL_TYPE_UNDEFINED && 
			SType < S_CHANNEL_TYPE_MAX) )
	{
		I3PRINTLOG(I3LOG_FATAL,  "SChannelType 에 등록 안 된값[ %d ]이 서버에서 날라온다.", SType );
		return 0;
	}

	return (INT32)m_Info[SType].size();
}


const CServerInfo *	CServerInfoContext::GetServerInfo(UINT32 ServerIndex, SChannelType SType /* = S_CHANNEL_TYPE_UNKNOWN */) const
{
	if ( ServerIndex >= static_cast<UINT32>(m_Info[SType].size()) )
		return nullptr;

	return m_Info[SType][ServerIndex];
}

void	CServerInfoContext::CreateServerInfo(bool UseServerSelect, UINT32 ServerCount)
{
	Clear();

	I3ASSERT(ServerCount > 0);
	m_IsUseServerSelect		= UseServerSelect;
	m_Info[S_CHANNEL_TYPE_ALL].resize(ServerCount);

	for ( UINT32 i = 0 ; i < ServerCount ; ++i )
		m_Info[S_CHANNEL_TYPE_ALL][i] = CServerInfo::new_object();

	m_IsUseServerSelect = true;
}

void	CServerInfoContext::GetIndexByTypeAndType(UINT32 uiAll_Index, UINT32& uiIndex, SChannelType& eType)
{
	const CServerInfo * pServerInfo = GetServerInfo(uiAll_Index, S_CHANNEL_TYPE_ALL);
	const CServerInfo * pServerInfoTemp = nullptr;


	for ( INT32 i = S_CHANNEL_TYPE_NORMAL ; i < S_CHANNEL_TYPE_MAX ; i++ ) {
		UINT32 uiCount = m_Info[i].size();
		
		for(UINT32 j=0; j<uiCount; j++) {
			pServerInfoTemp = m_Info[i][j];
			if(pServerInfoTemp == pServerInfo) {
				uiIndex = j;
				eType = (SChannelType)i;
				return;
			}
		}
	}
	uiIndex = 0xffffffff;
	eType = S_CHANNEL_TYPE_UNDEFINED;
}

bool	CServerInfoContext::UpdateServerInfo(UINT32 ServerCount, GAME_SERVER_INFO * pInfoArr)
{
	I3ASSERT( ServerCount > 0 );
	I3ASSERT( ServerCount == static_cast<UINT32>(m_Info[S_CHANNEL_TYPE_ALL].size()) );

	// 채널 정보를 해제시킵니다. 다시 만들기 위해서입니다.
	ClearServerInfos();

	for ( UINT32 i = 0 ; i < ServerCount ; ++i )
	{
		CServerInfo * pServerInfo = m_Info[S_CHANNEL_TYPE_ALL][i];
		if ( ! pServerInfo )
			continue;

		pServerInfo->Set(i, pInfoArr[i]);

		SChannelType	ServerType = static_cast<SChannelType>(pInfoArr[i]._AllType);
		if ( ServerType >= 0 && ServerType < S_CHANNEL_TYPE_MAX )
		{
	#if defined( LOCALE_INDONESIA)
			if( ServerType == S_CHANNEL_TYPE_PCCAFE_SILVER || ServerType == S_CHANNEL_TYPE_PCCAFE_GOLD)
			{//인도네시아 SILVER, GLOD는 Normal 타입 리스트에 넣음.
				ServerType = S_CHANNEL_TYPE_NORMAL;
			}
			else if( ServerType == S_CHANNEL_TYPE_PCCAFE_CLAN)
			{//인도네시아 GWClan 은 Clan 타입 리스트에 넣음.
				ServerType = S_CHANNEL_TYPE_CLAN;
			}
	#endif
			m_Info[ServerType].push_back(pServerInfo);
		}

#if defined(I3_DEBUG) && 0
		I3TRACE("SERVER_INFO_GAME %02d\n", i);
		I3TRACE("  IsActive : %d\n", pInfoArr[i]._IsActive);
		in_addr	in, out;
		in.S_un.S_addr = pInfoArr[i]._InIP;
		out.S_un.S_addr = pInfoArr[i]._OutIP;
		I3TRACE("  OutIP    : %s / %d\n", inet_ntoa(out), pInfoArr[i]._OutIP);
		I3TRACE("  OutPort  : %d\n", pInfoArr[i]._OutPort);		
		I3TRACE("  AllType  : %d     / pSession : %d\n", pInfoArr[i]._AllType, pInfoArr[i]._pSession);
#endif
	}

	return true;
}


void	CServerInfoContext::Clear()
{
//	m_Info[S_CHANNEL_TYPE_ALL].SafeReleaseAll();
	i3::cu::for_each_safe_release_clear( m_Info[S_CHANNEL_TYPE_ALL], &CServerInfo::Release);
	ClearServerInfos();
}


void	CServerInfoContext::ClearServerInfos()
{
	for ( INT32 i = S_CHANNEL_TYPE_NORMAL ; i < S_CHANNEL_TYPE_MAX ; ++i )
		m_Info[i].clear();
}

INT32 CServerInfoContext::GetUseIPCount()
{
	return m_pServerIPManager->GetCount();
}

INT32 CServerInfoContext::GetCurrentIPIdx()
{
	return m_pServerIPManager->GetCurrentIPIdx();
}

void CServerInfoContext::SetCurrentIPIdx(INT32 idx)
{
	return m_pServerIPManager->SetCurrentIPIdx(idx);
}

const char * CServerInfoContext::GetUseServerSet() const
{
	return m_pServerIPManager->m_strUseServerSet;
}

char * CServerInfoContext::GetCompareLauncherStr() const
{
	return m_pServerIPManager->GetCompareIP();
}

const char *	CServerInfoContext::GetLogInServerIPStr(INT32 idx) const
{
	const char* pszIP = m_pServerIPManager->GetIP(idx);
	return pszIP;
}

const char *	CServerInfoContext::GetLogInServerIPStr() const
{
	const char* pszIP = m_pServerIPManager->GetCurrentIP();

	if( _IsDottedAddressIP( pszIP ) )
		return pszIP;

	else
		return _ConvertDomainName2IP( pszIP );
}


UINT32		CServerInfoContext::GetLogInServerIP() const
{
	const char* pszIP = m_pServerIPManager->GetCurrentIP();
	
	if( _IsDottedAddressIP( pszIP ) )
		return ::inet_addr( pszIP );

	else
		return ::inet_addr( _ConvertDomainName2IP(pszIP) );
}

UINT16		CServerInfoContext::GetLogInServerPort() const
{
	return SERVER_PORT_TCP;
}

INT32		CServerInfoContext::GetGameServerIndex(INT32 ServerIndex, SChannelType SType /*= S_CHANNEL_TYPE_UNKNOWN*/)
{
	I3ASSERT( ServerIndex >= 0 && ServerIndex < (INT32)m_Info[SType].size() );
	const CServerInfo * pServerInfo = GetServerInfo(ServerIndex, SType);
	if ( ! pServerInfo )
		return -1;

	return pServerInfo->GetNumber();
}

bool		CServerInfoContext::SetGameServer(INT32 ServerIndex, SChannelType SType /*= S_CHANNEL_TYPE_UNKNOWN*/)
{
	const CServerInfo * pServerInfo = GetServerInfo(ServerIndex, SType);
	if ( ! pServerInfo )
	{
		m_GameServerIndexByType	= -1;
		m_GameServerType		= S_CHANNEL_TYPE_UNDEFINED;
		return false;
	}

	m_GameServerIndexByType	= ServerIndex;
	SetCurGameServerType(SType);
	
	return true;
}

const CServerInfo *			CServerInfoContext::GetCurGameServer() const
{
	return GetServerInfo(m_GameServerIndexByType, m_GameServerType);
}

INT32			CServerInfoContext::GetCurGameServerIndex() const
{
	const CServerInfo * pServerInfo = GetCurGameServer();
	if ( pServerInfo )
		return (INT32)pServerInfo->GetNumber();
	return -1;
}

INT32 CServerInfoContext::GetServerNumber( void )
{
	return (this->GetCurGameServerIndex() * 10) + ( this->GetCurChannelNumber() + 1 );
}


