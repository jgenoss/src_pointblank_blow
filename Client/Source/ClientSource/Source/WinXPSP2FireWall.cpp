#include "pch.h"
#include "winxpsp2firewall.h"

WinXPSP2FireWall::WinXPSP2FireWall(void)
{
	m_pFireWallProfile = nullptr;
}

WinXPSP2FireWall::~WinXPSP2FireWall(void)
{
	Uninitialize();
}

FW_ERROR_CODE WinXPSP2FireWall::Initialize()
{
	HRESULT hr = S_FALSE;
	INetFwMgr* fwMgr = nullptr;
	INetFwPolicy* fwPolicy = nullptr;

	FW_ERROR_CODE ret = FW_NOERROR;
	try
	{
		if( m_pFireWallProfile )
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall opening failed: 1 hr %d ", hr);
			throw FW_ERR_INITIALIZED;
		}

		// Create an instance of the firewall settings manager.
		hr = ::CoCreateInstance( __uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER, __uuidof( INetFwMgr), (void**)&fwMgr );

		if( FAILED( hr ))
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall opening failed: 2 hr %d ", hr);
			throw FW_ERR_CREATE_SETTING_MANAGER;
		}

		// Retrieve the local firewall policy.
		hr = fwMgr->get_LocalPolicy( &fwPolicy );
		if( FAILED( hr ))
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall opening failed: 3 hr %d ", hr);
			throw FW_ERR_LOCAL_POLICY;
		}

		// Retrieve the firewall profile currently in effect
		hr = fwPolicy->get_CurrentProfile( &m_pFireWallProfile );
		if( FAILED( hr ))
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall opening failed: 4 hr %d ", hr);
			if(hr == E_ACCESSDENIED){
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall Access denied");
			}
			else if(hr == E_OUTOFMEMORY){
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall Out of memory");
			}
			else if(hr == E_POINTER){
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall Pointer error");
			}
			else{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall Result : None");
			}
			throw FW_ERR_PROFILE;
		}

	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	if( fwPolicy )
		fwPolicy->Release();
	if( fwMgr )
		fwMgr->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::Uninitialize()
{
	// Release the firewall profile
	if( m_pFireWallProfile )
	{
		m_pFireWallProfile->Release();
		m_pFireWallProfile = nullptr;
	}

	return FW_NOERROR;
}

FW_ERROR_CODE WinXPSP2FireWall::IsWindowsFirewallOn( bool& bOn )
{
	HRESULT hr;
	VARIANT_BOOL bFWEnabled;
	bOn = false;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		hr = m_pFireWallProfile->get_FirewallEnabled( &bFWEnabled );
		if( FAILED(hr))
			throw FW_ERR_FIREWALL_IS_ENABLED;

		if( bFWEnabled != VARIANT_FALSE )
			bOn = true;
	}
	catch( FW_ERROR_CODE nError )
	{
		return nError;
	}

	return FW_NOERROR;
}

FW_ERROR_CODE WinXPSP2FireWall::TurnOnWindowsFirewall()
{
	HRESULT hr;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		// Check whether the firewall is off
		bool bFWOn;
		FW_ERROR_CODE ret = IsWindowsFirewallOn( bFWOn );

		if( ret != FW_NOERROR )
			throw ret;

		// If it is off now, turn it on
		if( !bFWOn )
		{
			hr = m_pFireWallProfile->put_FirewallEnabled( VARIANT_TRUE );
			if( FAILED( hr ))
				throw FW_ERR_FIREWALL_ENABLED;
		}
	}
	catch( FW_ERROR_CODE nError )
	{
		return nError;
	}

	return FW_NOERROR;
}

FW_ERROR_CODE WinXPSP2FireWall::TurnOffWindowsFirewall()
{
	HRESULT hr;
	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		// Check whether the firewall is off
		bool bFWOn;
		FW_ERROR_CODE ret = IsWindowsFirewallOn( bFWOn );

		if( ret != FW_NOERROR )
			throw ret;

		// If it is on now, turn it off
		if( bFWOn )
		{
			hr = m_pFireWallProfile->put_FirewallEnabled( VARIANT_FALSE );
			if( FAILED( hr ))
				throw FW_ERR_FIREWALL_ENABLED;
		}
	}
	catch( FW_ERROR_CODE nError )
	{
		return nError;
	}
	return FW_NOERROR;
}

FW_ERROR_CODE WinXPSP2FireWall::IsAppEnabled( const wchar_t* lpszProcessImageFileName, bool& bEnable )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	HRESULT hr;
	BSTR bstrFWProcessImageFileName = nullptr;
	VARIANT_BOOL bFWEnabled;
	INetFwAuthorizedApplication* pFWApp = nullptr;
	INetFwAuthorizedApplications* pFWApps = nullptr;
	
	bEnable = false;
	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		if( lpszProcessImageFileName == nullptr )
			throw FW_ERR_INVALID_ARG;

		hr = m_pFireWallProfile->get_AuthorizedApplications( &pFWApps );
		if( FAILED( hr ))
			throw FW_ERR_AUTH_APPLICATIONS;

		// Allocate a BSTR for the process image file name
		bstrFWProcessImageFileName = SysAllocString( lpszProcessImageFileName );
		if( SysStringLen( bstrFWProcessImageFileName ) == 0)
			throw FW_ERR_SYS_ALLOC_STRING;

		hr = pFWApps->Item( bstrFWProcessImageFileName, &pFWApp);
		// If FAILED, the appliacation is not in the collection list
		if( SUCCEEDED( hr ))
		{
			// Find out if the authorized application is enabled
			hr = pFWApp->get_Enabled( &bFWEnabled );

			if( FAILED( hr ))
				throw FW_ERR_APP_ENABLED;

			if( bFWEnabled == VARIANT_TRUE )
				bEnable = true;
		}
	}
	catch( FW_ERROR_CODE nError )
	{
		ret = nError;
	}
	
	// Free the BSTR
	SysFreeString( bstrFWProcessImageFileName );

	// Release memories to retrieve the information of the application
	if( pFWApp )
		pFWApp->Release();
	if( pFWApps )
		pFWApps->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::AddApplication( const wchar_t* lpszProcessImageFileName, const wchar_t* lpszRegisterName )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	HRESULT hr;
	bool bAppEnable;
	BSTR bstrProcessImageFileName = nullptr;
	BSTR bstrRegisterName = nullptr;
	INetFwAuthorizedApplication* pFWApp = nullptr;
	INetFwAuthorizedApplications* pFWApps = nullptr;

	try
	{
		if( m_pFireWallProfile == nullptr )
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 1");
			throw FW_ERR_INITIALIZED;
		}

		if( lpszProcessImageFileName == nullptr || lpszRegisterName  == nullptr )
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 2");
			throw FW_ERR_INVALID_ARG;
		}

		// First of all, check the application is already authorized;
		FW_ERROR_CODE  nError = this->IsAppEnabled( lpszProcessImageFileName, bAppEnable );
		if( nError != FW_NOERROR )
		{
			I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 3");
			throw nError;
		}

		// Only add the application if it isn't authorized
		if( bAppEnable == false )
		{
			// Retrieve the authorized application collection
			hr = m_pFireWallProfile->get_AuthorizedApplications( &pFWApps );
			if( FAILED( hr ))
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 4");
				throw FW_ERR_AUTH_APPLICATIONS;
			}

			// Create an instance of an authorized application
			hr = ::CoCreateInstance( __uuidof(NetFwAuthorizedApplication), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwAuthorizedApplication), (void**)&pFWApp);
			if( FAILED( hr ))
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 5");
				throw FW_ERR_CREATE_APP_INSTANCE;
			}

			// Allocate a BSTR for the Process Image FileName
			bstrProcessImageFileName = SysAllocString( lpszProcessImageFileName );
			if( SysStringLen( bstrProcessImageFileName ) == 0)
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 6");
				throw FW_ERR_SYS_ALLOC_STRING;
			}

			// Set the process image file name
			hr = pFWApp->put_ProcessImageFileName( bstrProcessImageFileName );
			if( FAILED( hr ) )
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 7");
				throw FW_ERR_PUT_PROCESS_IMAGE_NAME;
			}

			// Allocate a BSTR for register name
			bstrRegisterName = SysAllocString( lpszRegisterName );
			if( SysStringLen( bstrRegisterName ) == 0)
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 6");
				throw FW_ERR_SYS_ALLOC_STRING;
			}

			// Set a registered name of the process
			hr = pFWApp->put_Name( bstrRegisterName );
			if( FAILED( hr ))
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 7");
				throw FW_ERR_PUT_REGISTER_NAME;
			}
			
			// Add the application to the collection
			hr = pFWApps->Add( pFWApp );
			if( FAILED( hr ))
			{
				I3PRINTLOG(I3LOG_NOTICE, "Windows XP Firewall add application : 8");
				throw FW_ERR_ADD_TO_COLLECTION;
			}
		}
	}
	catch( FW_ERROR_CODE nError )
	{
		ret = nError;
	}

	SysFreeString( bstrProcessImageFileName );
	SysFreeString( bstrRegisterName );

	if( pFWApp )
		pFWApp->Release();
	if( pFWApps )
		pFWApps->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::RemoveApplication( const wchar_t* lpszProcessImageFileName )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	HRESULT hr;
	bool bAppEnable;
	BSTR bstrProcessImageFileName = nullptr;
	INetFwAuthorizedApplications* pFWApps = nullptr;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;
		if( lpszProcessImageFileName == nullptr )
			throw FW_ERR_INVALID_ARG;

		FW_ERROR_CODE  nError = this->IsAppEnabled( lpszProcessImageFileName, bAppEnable );
		if( nError != FW_NOERROR )
			throw nError;

		// Only remove the application if it is authorized
		if( bAppEnable == true )
		{
			// Retrieve the authorized application collection
			hr = m_pFireWallProfile->get_AuthorizedApplications( &pFWApps );
			if( FAILED( hr ))
				throw FW_ERR_AUTH_APPLICATIONS;

			// Allocate a BSTR for the Process Image FileName
			bstrProcessImageFileName = SysAllocString( lpszProcessImageFileName );
			if( SysStringLen( bstrProcessImageFileName ) == 0)
				throw FW_ERR_SYS_ALLOC_STRING;
			hr = pFWApps->Remove( bstrProcessImageFileName );
			if( FAILED( hr ))
				throw FW_ERR_REMOVE_FROM_COLLECTION;
		}
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	SysFreeString( bstrProcessImageFileName);
	if( pFWApps )
		pFWApps->Release();

	return ret;
}
FW_ERROR_CODE WinXPSP2FireWall::IsPortEnabled( LONG lPortNumber, NET_FW_IP_PROTOCOL ipProtocol, bool& bEnable )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	VARIANT_BOOL bFWEnabled;
	INetFwOpenPort* pFWOpenPort = nullptr;
	INetFwOpenPorts* pFWOpenPorts = nullptr;
	HRESULT hr;

	bEnable = false;
	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		// Retrieve the open ports collection
		hr = m_pFireWallProfile->get_GloballyOpenPorts( &pFWOpenPorts );
		if( FAILED( hr ))
			throw FW_ERR_GLOBAL_OPEN_PORTS;

		// Get the open port
		hr = pFWOpenPorts->Item( lPortNumber, ipProtocol, &pFWOpenPort );
		if( SUCCEEDED( hr ))
		{
			hr = pFWOpenPort->get_Enabled( &bFWEnabled );
			if( FAILED( hr ))
				throw FW_ERR_PORT_IS_ENABLED;

			if( bFWEnabled == VARIANT_TRUE )
				bEnable = true;
		}
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	if( pFWOpenPort )
		pFWOpenPort->Release();
	if( pFWOpenPorts )
		pFWOpenPorts->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::AddPort( LONG lPortNumber, NET_FW_IP_PROTOCOL ipProtocol, const wchar_t* lpszRegisterName )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	INetFwOpenPort* pFWOpenPort = nullptr;
	INetFwOpenPorts* pFWOpenPorts = nullptr;
	BSTR bstrRegisterName = nullptr;
	HRESULT hr;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;
		bool bEnablePort;
		FW_ERROR_CODE nError = IsPortEnabled( lPortNumber, ipProtocol, bEnablePort);
		if( nError != FW_NOERROR)
			throw nError;

		// Only add the port, if it isn't added to the collection
		if( bEnablePort == false )
		{
			// Retrieve the collection of globally open ports
			hr = m_pFireWallProfile->get_GloballyOpenPorts( &pFWOpenPorts );
			if( FAILED( hr ))
				throw FW_ERR_GLOBAL_OPEN_PORTS;

			// Create an instance of an open port
			hr = ::CoCreateInstance( __uuidof(NetFwOpenPort), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwOpenPort), (void**)&pFWOpenPort);
			if( FAILED( hr ))
				throw FW_ERR_CREATE_PORT_INSTANCE;

			// Set the port number
			hr = pFWOpenPort->put_Port( lPortNumber );
			if( FAILED( hr ))
				throw FW_ERR_SET_PORT_NUMBER;

			// Set the IP Protocol
			hr = pFWOpenPort->put_Protocol( ipProtocol );
			if( FAILED( hr ))
				throw FW_ERR_SET_IP_PROTOCOL;

			bstrRegisterName = SysAllocString( lpszRegisterName );
			if( SysStringLen( bstrRegisterName ) == 0)
				throw FW_ERR_SYS_ALLOC_STRING;
		
			// Set the registered name
			hr = pFWOpenPort->put_Name( bstrRegisterName );
			if( FAILED( hr ))
				throw FW_ERR_PUT_REGISTER_NAME;

			hr = pFWOpenPorts->Add( pFWOpenPort );
			if( FAILED( hr ))
				throw FW_ERR_ADD_TO_COLLECTION;
		}

	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	SysFreeString( bstrRegisterName );
	if( pFWOpenPort )
		pFWOpenPort->Release();
	if( pFWOpenPorts )
		pFWOpenPorts->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::RemovePort( LONG lPortNumber, NET_FW_IP_PROTOCOL ipProtocol )
{
	FW_ERROR_CODE ret = FW_NOERROR;
	INetFwOpenPorts* pFWOpenPorts = nullptr;
	HRESULT hr;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;
		bool bEnablePort;
		FW_ERROR_CODE nError = IsPortEnabled( lPortNumber, ipProtocol, bEnablePort);
		if( nError != FW_NOERROR)
			throw nError;

		// Only remove the port, if it is on the collection
		if( bEnablePort == true )
		{
			// Retrieve the collection of globally open ports
			hr = m_pFireWallProfile->get_GloballyOpenPorts( &pFWOpenPorts );
			if( FAILED( hr ))
				throw FW_ERR_GLOBAL_OPEN_PORTS;

			hr = pFWOpenPorts->Remove( lPortNumber, ipProtocol );
			if (FAILED( hr ))
				throw FW_ERR_REMOVE_FROM_COLLECTION;
		}

	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	if( pFWOpenPorts )
		pFWOpenPorts->Release();

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::IsExceptionNotAllowed( bool& bNotAllowed )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	bNotAllowed = true;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		VARIANT_BOOL bExNotAllowed;

		HRESULT hr = m_pFireWallProfile->get_ExceptionsNotAllowed( &bExNotAllowed );
		
		if( FAILED( hr ))
			throw FW_ERR_EXCEPTION_NOT_ALLOWED;
		
		if( bExNotAllowed == VARIANT_TRUE )
			bNotAllowed = true;
		else
			bNotAllowed = false;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::SetExceptionNotAllowed( bool bNotAllowed )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;
		HRESULT hr = m_pFireWallProfile->put_ExceptionsNotAllowed( bNotAllowed ? VARIANT_TRUE : VARIANT_FALSE );

		if( FAILED( hr ))
			throw FW_ERR_EXCEPTION_NOT_ALLOWED;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::IsNotificationDiabled( bool& bDisabled )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	bDisabled = false;
	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		VARIANT_BOOL bNotifyDisable;
		HRESULT hr = m_pFireWallProfile->get_NotificationsDisabled( &bNotifyDisable );
		if( FAILED( hr ))
			throw FW_ERR_NOTIFICATION_DISABLED;
		
		if( bNotifyDisable == VARIANT_TRUE )
			bDisabled = true;
		else
			bDisabled = false;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::SetNotificationDiabled( bool bDisabled )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		HRESULT hr = m_pFireWallProfile->put_NotificationsDisabled( bDisabled ? VARIANT_TRUE : VARIANT_FALSE );
		if( FAILED( hr ))
			throw FW_ERR_NOTIFICATION_DISABLED;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::IsUnicastResponsesToMulticastBroadcastDisabled( bool& bDisabled )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	bDisabled = false;
	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		VARIANT_BOOL bUniMultiDisabled;
		HRESULT hr = m_pFireWallProfile->get_UnicastResponsesToMulticastBroadcastDisabled( &bUniMultiDisabled );
		if( FAILED( hr ))
			throw FW_ERR_UNICAST_MULTICAST;

		if( bUniMultiDisabled == VARIANT_TRUE )
			bDisabled = true;
		else
			bDisabled = false;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}

FW_ERROR_CODE WinXPSP2FireWall::SetUnicastResponsesToMulticastBroadcastDisabled( bool bDisabled )
{
	FW_ERROR_CODE ret = FW_NOERROR;

	try
	{
		if( m_pFireWallProfile == nullptr )
			throw FW_ERR_INITIALIZED;

		HRESULT hr = m_pFireWallProfile->put_UnicastResponsesToMulticastBroadcastDisabled( bDisabled ? VARIANT_TRUE : VARIANT_FALSE );
		if( FAILED( hr ))
			throw FW_ERR_UNICAST_MULTICAST;
	}
	catch( FW_ERROR_CODE nError)
	{
		ret = nError;
	}

	return ret;
}
