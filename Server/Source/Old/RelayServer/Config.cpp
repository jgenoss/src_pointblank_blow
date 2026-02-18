#include "pch.h"
#include "Config.h"

CConfig * g_pConfig = NULL;

I3_CLASS_INSTANCE( CConfig, i3ElementBase);

CConfig::CConfig(void)
{
	m_LocalTime					= 0.0f; 
	m_iServerTime				= 10000; //계산을 위해서 이 값부터 시작... UINT32 때문
	m_UdpIp						= 0;
	m_UdpActiveIp				= 0;
	m_cUdpPortCount				= 0;
	m_pUdpPort					= NULL;

	m_MacAddressListCount		= 0; 
	m_pMacAddressList			= NULL;

	m_connectedGameServerCount	= 0;
	m_connectedUserCount		= 0;

	m_pServerIp					= NULL;

	m_wGroupCount				= 0;
}

CConfig::~CConfig(void)
{
	I3MEM_SAFE_FREE( m_pUdpPort );
	I3MEM_SAFE_FREE( m_pMacAddressList ); 
	I3MEM_SAFE_FREE( m_pServerIp );
}

BOOL CConfig::LoadFromFile(const char * szFileName)
{
	if( szFileName == NULL)	return FALSE;

	i3IniParser Parser;

	if( !Parser.OpenFromFile( szFileName))
	{		
		return FALSE;
	}

	INT32 Temp = 0;  
	//Read Default 
	if( Parser.ReadSection("Default") )
	{       
		strcpy( m_LogFilePath, Parser.GetStringValue("LogFilePath") );

		char szUdpIP[ MAX_PATH ];
		strcpy( szUdpIP,		Parser.GetStringValue("UdpIp"));
		m_UdpIp			= ::inet_addr( szUdpIP );

		strcpy( szUdpIP,		Parser.GetStringValue("UdpIpActive"));
		m_UdpActiveIp	= ::inet_addr( szUdpIP );
	}
	else
	{
		return FALSE; 
	}

	//LogD
	if( Parser.ReadSection("LogD"))
	{
		Parser.GetValue( "LogDActive", Temp, &Temp);
		if( Temp == 0)	m_bLogDActive = FALSE; 
		else			m_bLogDActive = TRUE; 

		strcpy(m_LogDServerIp, Parser.GetStringValue("LogDServerIp"));
		Parser.GetValue( "LogDServerPort", Temp, &Temp);
		m_LogDServerPort = (UINT16)Temp;
	}else return FALSE; 

	//Control
	if( Parser.ReadSection("Control"))
	{
		m_bControlActive = TRUE;

		strcpy(m_ControlServerIp, Parser.GetStringValue("ControlServerIp"));
		Parser.GetValue( "ControlServerPort", Temp, &Temp);
		m_ControlServerPort = (UINT16)Temp; 
	}else return FALSE; 

	Parser.Close();

	return TRUE;
}

UINT32 CConfig::GetConnectedGameServerCount(void) const
{
	return m_connectedGameServerCount;
}

UINT32 CConfig::GetConnectedUserCount(void) const
{
	return m_connectedUserCount;
}

void CConfig::AddConnectedGameServer(void)
{
	if (m_connectedGameServerCount < m_ServerCount)
	{
		m_connectedGameServerCount++;
	}
}

void CConfig::SubConnectedGameServer(void)
{
	if (0 < m_connectedGameServerCount)
	{
		m_connectedGameServerCount--;
	}
}

void CConfig::SetConnectedUserCount(UINT32 count)
{
	m_connectedUserCount = count;
}
