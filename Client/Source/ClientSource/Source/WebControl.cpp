#include "pch.h"
#include "WebControl.h"
//#import "C:\windows\System32\mshtml.tlb"

#include "SteamContext.h"
#include "UI/UIServerAndChannelBanner.h"
#include "UI/UIUtil.h"

CWebContainer::CWebContainer( HWND hwnd )
{
	m_cRef = 0;
	m_hwnd = hwnd;
	m_punk = nullptr;

	SetRectEmpty( &m_rect );

	m_pEvent = new CWebEventSink( hwnd);

	m_pEvent->AddRef();

	m_rect.bottom = 0;
	m_rect.left = 0;
	m_rect.right = 0;
	m_rect.top = 0;
	m_eventCookie = 0;
}

CWebContainer::~CWebContainer()
{
	m_pEvent->Release();

	m_punk->Release();

}


void CWebEventSink::OnNavigateError( DISPPARAMS *pDispParams)
{
	//Extract the status code from the DISPPARAMS structure
	VARIANT * vt_statuscode = pDispParams->rgvarg[1].pvarVal;
	DWORD  dwStatusCode =  vt_statuscode->lVal;

	//INET_E_INVALID_URL
	//INET_E_RESOURCE_NOT_FOUND

	CWebCtrl *ctl	= nullptr;
	ctl = (CWebCtrl*) GetProp( m_hwnd, "this");
	if( ctl)
	{
		ctl->SethrNavigate2( S_FALSE);
		ctl->SethrNavigate2ErrorCode( dwStatusCode);

		// 배너 끄자.
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_WEBCONTROL_ERROR);
	}


	//Extract the event's IDispatch pointer
	//IDispatch *pdispFiredEvent = pDispParams->rgvarg[4].pdispVal;
	//
}

void CWebEventSink::OnDocumentComplete()
{
	CWebCtrl *ctl	= nullptr;
	ctl = (CWebCtrl*) GetProp( m_hwnd, "this");
	if( ctl)
	{
		ctl->OnDocumentComplete();

		//CComVariant  vv;
		//vv = 200;
		//ctl->GetWebBrowser2()->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &vv, nullptr);
	}
}

//새로운 창으로 못열게 설정
void CWebEventSink::CloseNewWebbrowser(DISPPARAMS *ppDisp)
{
	CWebCtrl *ctl = nullptr;
	ctl = (CWebCtrl*)GetProp(m_hwnd, "this");
	if (ctl)
	{
		if		(ctl->isCloseNewWebbrowser())	*(ppDisp->rgvarg[0].pboolVal) = true;
		else if (ctl->isOpenSteamWebbrowser())	*(ppDisp->rgvarg[0].pboolVal) = true;
		else if (ctl->isPBWebboxuibrowser())	*(ppDisp->rgvarg[0].pboolVal) = true;
	}
}

//현재 webbrowser로 연다
void CWebEventSink::OpenCurrentWebbrowser(DISPPARAMS *ppDisp)
{
	CWebCtrl *ctl = nullptr;
	ctl = (CWebCtrl*)GetProp(m_hwnd, "this");
	if (ctl)
	{
		VARIANT v;
		VariantInit(&v);

		v.vt = VT_BSTR;
		v.bstrVal = ppDisp->rgvarg[0].bstrVal;	//열려고하는 url
		
		char* pbstr;
		USES_CONVERSION;								// <atlconv.h>, convert를 위한 macro
		pbstr = OLE2A(ppDisp->rgvarg[0].bstrVal);       // BSTR -> char

		if (ctl->isOpenSteamWebbrowser())
		{
			if (SteamContext::i()->IsSteamUser())
			{
				SteamContext::i()->ActivateWebPage(pbstr);
			}
			else
			{
				ctl->GetWebBrowser2()->Navigate2(&v, nullptr, nullptr, nullptr, nullptr);
			}
		}
		else if (ctl->isCloseNewWebbrowser())
		{
			ctl->GetWebBrowser2()->Navigate2(&v, nullptr, nullptr, nullptr, nullptr);
		}
		else if (ctl->isPBWebboxuibrowser())
		{
			g_pFramework->GetUIMainframe()->OpenPopup(UPW_WEB_BOX, pbstr);
		}
		else
		{
			return;
		}
	}
}

void CWebContainer::add()
{
	CLSID clsid;

	CLSIDFromString( L"Shell.Explorer", &clsid );

	CoCreateInstance( clsid, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (PVOID *) &m_punk );

	IOleObject *pioo;

	m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

	pioo->SetClientSite( this );

	pioo->Release();

	IPersistStreamInit *ppsi;

	m_punk->QueryInterface( IID_IPersistStreamInit, (PVOID *) &ppsi );

	ppsi->InitNew();
	ppsi->Release();

	ConnectEvents();

	setVisible( true );

	setFocus( true );
}

/**************************************************************************

CWebContainer::remove()

**************************************************************************/

void CWebContainer::remove()
{
	IOleObject *pioo;

	m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

	pioo->Close( OLECLOSE_NOSAVE );

	pioo->SetClientSite( nullptr );

	pioo->Release();

	IOleInPlaceObject *pipo;

	m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

	pipo->UIDeactivate();

	pipo->InPlaceDeactivate();

	pipo->Release();

	DisconnectEvents();
}

/**************************************************************************

CWebContainer::setLocation()

**************************************************************************/

void CWebContainer::setLocation( int x, int y, int cx, int cy )
{
	m_rect.left   = x;
	m_rect.top    = y;
	m_rect.right  = cx;
	m_rect.bottom = cy;

	IOleInPlaceObject *pipo;

	m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

	pipo->SetObjectRects( &m_rect, &m_rect );

	pipo->Release();
}

/**************************************************************************

CWebContainer::setVisible()

**************************************************************************/

void CWebContainer::setVisible( bool bVisible )
{
	IOleObject *pioo;

	m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

	if ( bVisible )
	{
		pioo->DoVerb( OLEIVERB_INPLACEACTIVATE, nullptr, this, 0, m_hwnd, &m_rect );
		pioo->DoVerb( OLEIVERB_SHOW, nullptr, this, 0, m_hwnd, &m_rect );
	}
	else
		pioo->DoVerb( OLEIVERB_HIDE, nullptr, this, 0, m_hwnd, nullptr );

	pioo->Release();
}

/**************************************************************************

CWebContainer::setFocus()

**************************************************************************/

void CWebContainer::setFocus( bool bFocus )
{
	IOleObject *pioo;

	if ( bFocus )
	{
		m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

		pioo->DoVerb( OLEIVERB_UIACTIVATE, nullptr, this, 0, m_hwnd, &m_rect );

		pioo->Release();
	}
}

void CWebContainer::ConnectEvents()
{
	IConnectionPoint *pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

	pcp->Advise( reinterpret_cast<IDispatch*>(this), &m_eventCookie );

	pcp->Release();
}

void CWebContainer::DisconnectEvents()
{
	IConnectionPoint *pcp;

	pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

	pcp->Unadvise( m_eventCookie );

	pcp->Release();
}

IConnectionPoint* CWebContainer::GetConnectionPoint( REFIID riid )
{
	IConnectionPointContainer *pcpc;
	HRESULT	hResult;

	m_punk->QueryInterface( IID_IConnectionPointContainer, (PVOID *) &pcpc );

	IConnectionPoint *pcp;

	hResult = pcpc->FindConnectionPoint( riid, &pcp );
	if(SUCCEEDED(hResult))
	{
		/*hResult = pcp->Advise( reinterpret_cast<IDispatch*>(this), &m_eventCookie );
		pcp->Release();*/
	}

	pcpc->Release();

	return pcp;
}

HRESULT CWebContainer::getWeb( IWebBrowser2 **ppweb )
{
	//return m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) ppweb );
	return m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) ppweb );
}

// WebCtrl

CWebCtrl::CWebCtrl( HWND hwnd )
{
	m_hwnd = hwnd;
	m_cxScroll = GetSystemMetrics(SM_CXHSCROLL) + 2;
	m_cyScroll = GetSystemMetrics(SM_CYVSCROLL) + 2;
	m_pContainer = nullptr;
	m_pweb = nullptr;
	m_hrNavigate2 = S_FALSE;
	m_dwErrorCode = 0;

	cb_docmuent_complete = 0;

	m_bCloseNewWebbrowser	= false;
	m_bSteamWebbrowser		= false;		
	m_bPBWebboxuibrowser	= false;

}

LRESULT CALLBACK WebCtrlProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CWebCtrl *pthis	= nullptr;

	pthis	= (CWebCtrl *) GetProp( hwnd, "this" );

	if ( !pthis )
	{
		if ( uMsg != WM_CREATE )
			return DefWindowProc( hwnd, uMsg, wParam, lParam );

		pthis = new CWebCtrl( hwnd );

		SetProp( hwnd, "this", (HANDLE) pthis );

	}

	LRESULT lResult = pthis->WebCtrlProc( uMsg, wParam, lParam );

	if ( uMsg == WM_DESTROY)
	{
		RemoveProp( hwnd, "this" );
		delete pthis;
	}

	return lResult;
}

void CWebCtrl::Navigate( LPTSTR psz )
{
	if ( !psz )
		return;

	if ( !*psz )
		return;

	int len = lstrlen( psz ) + 1;

	WCHAR *pszW;

#ifndef UNICODE

	pszW = new WCHAR[len];

	i3String::MultiByteToWideChar( CP_ACP, 0, psz, -1, pszW, len );

#else

	pszW = psz;

#endif

	VARIANT v;

	VariantInit( &v );

	v.vt = VT_BSTR;
	v.bstrVal = SysAllocString( pszW );

#ifndef UNICODE

	delete []pszW;

#endif

	m_hrNavigate2 = m_pweb->Navigate2( &v, nullptr, nullptr, nullptr, nullptr );

	VARIANT_BOOL v_b;
	m_pweb->get_Busy( &v_b);

	VariantClear( &v );
}

LRESULT CWebCtrl::WebCtrlProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = 0;
	
	switch ( uMsg )
	{
	case WM_CREATE:
		{
		m_pContainer = new CWebContainer(m_hwnd);

		m_pContainer->AddRef();

		m_pContainer->add();

		m_pContainer->getWeb( &m_pweb );
	
		CREATESTRUCT *pcs;

		pcs = (CREATESTRUCT *) lParam;

		Navigate( (LPTSTR) pcs->lpszName );
		x = pcs->cx;
		y = pcs->cy;

		break;
		}
	case WM_SETTEXT:

		Navigate( (LPTSTR) lParam );

		break;

	case WM_SIZE:

		//m_pContainer->setLocation( 0, 0, 800,600);// LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );
		m_pContainer->setLocation(0,0, x, y );//LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );

		break;

	case WM_DESTROY:
		if(m_pweb != nullptr)
			m_pweb->Release();
		if(m_pContainer != nullptr)
		{
			m_pContainer->remove();
			m_pContainer->Release();
		}
		//{
		//	UIMainFrame * pFrame = nullptr;
		//	pFrame = g_pFramework->GetUIMainframe();
		//	pFrame->TogglePopup(UPW_WEB_BOX);
		//}
		break;
	default:

		lResult = DefWindowProc( m_hwnd, uMsg, wParam, lParam );

		break; 
	}

	return lResult;
}

bool WINAPI InitWebCtrl()
{
	::CoInitialize( NULL );

	WNDCLASS wc;

	HINSTANCE hinst = ::GetModuleHandle( NULL );

	if ( !GetClassInfo( hinst, WC_WEBCTRL, &wc ) )
	{
		memset( &wc, 0, sizeof(wc) );

		wc.style         = CS_DBLCLKS | CS_GLOBALCLASS | CS_NOCLOSE;
		wc.lpfnWndProc   = ::WebCtrlProc;
		wc.hInstance     = hinst;
		wc.hCursor       = ::LoadCursor( NULL, IDC_ARROW );
		wc.lpszClassName = WC_WEBCTRL;

		return RegisterClass( &wc ) ? true : false;
	}

	return true;
}

bool WINAPI UninitWebCtrl()
{
	WNDCLASS wc;

	bool bResult = false;

	if ( GetClassInfo( ::GetModuleHandle( NULL ), WC_WEBCTRL, &wc ) )
		bResult = UnregisterClass( WC_WEBCTRL, wc.hInstance ) ? true : false;

	CoUninitialize();

	return bResult;
}

BOOL CWebCtrl::GetHTMLInterface(IWebBrowser2** ppBrowser, IHTMLDocument2** ppHtmlDocument, IHTMLWindow2** ppHtmlWindow)
{
	HRESULT hr;

	IDispatch* pDisp = nullptr;

	*ppBrowser = m_pweb;		m_pweb->AddRef();
	hr = (*ppBrowser)->get_Document(&pDisp);

	if (FAILED(hr))
	{
		ATLTRACE(_T("get_Document failed.\n"));
		(*ppBrowser)->Release();
		(*ppBrowser) = nullptr;
		return false;
	}
	if(pDisp == nullptr)
		return false;

	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)ppHtmlDocument);

	if (FAILED(hr))
	{
		ATLTRACE(_T("IHTMLDocument query failed.\n")); 
		pDisp->Release();
		pDisp = nullptr;

		(*ppBrowser)->Release();
		(*ppBrowser) = nullptr;
		return false;
	}
	pDisp->Release();
	pDisp = nullptr;

	hr = (*ppHtmlDocument)->get_parentWindow(ppHtmlWindow);
	if (FAILED(hr))
	{
		ATLTRACE(_T("IHTMLWindow2 query failed. hr=0x08x\n"), hr); 
		(*ppHtmlDocument)->Release();
		(*ppHtmlDocument) = nullptr;
		(*ppBrowser)->Release();
		(*ppBrowser) = nullptr;
		return false;
	}

	return true;
}

LPDISPATCH CWebCtrl::GetHtmlDocument() const
{
	ASSERT(m_pweb != nullptr);

	LPDISPATCH result;
	HRESULT hr = m_pweb->get_Document(&result);
	if(FAILED(hr))
	{
		ASSERT(false);
		return nullptr;
	}

	return result;
}

bool CWebCtrl::ExecuteJScript3(const CString strFunc, const CStringArray& paramArray)
{
	HRESULT hr;

	CComQIPtr<IHTMLDocument> pDoc = GetHtmlDocument();
	if(pDoc == nullptr)
		return false;
	//Getting IDispatch for Java Script objects
	CComPtr<IDispatch> spScript;
	hr = pDoc->get_Script(&spScript);
	if(FAILED(hr)) return false;

	//Find dispid for given function in the object
	CComBSTR bstrMember(strFunc);
	DISPID dispid = 0;
	hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
	if(FAILED(hr)) return false;

	const int arraySize = paramArray.GetSize();
	//Putting parameters  
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs      = arraySize;
	dispparams.rgvarg     = new VARIANT[dispparams.cArgs];
	dispparams.cNamedArgs = 0;

	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	//Call JavaScript function         
	hr = spScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD, &dispparams, &vaResult, &excepInfo, &nArgErr);
	delete [] dispparams.rgvarg;
	if(FAILED(hr)) return false;

	return true;
}

void CWebCtrl::ExecuteJScript2(const CString strFunc, const CStringArray& paramArray)
{
	HRESULT hr;
	CComPtr<IHTMLElementCollection> spScript;
	IWebBrowser2* pBrowser = nullptr;
	IHTMLDocument2* pHtmlDocument = nullptr;
	IHTMLWindow2* pHtmlWindow = nullptr;
	CComVariant pVarResult;

	if (!GetHTMLInterface(&pBrowser, &pHtmlDocument, &pHtmlWindow))
		return;

	hr = pHtmlDocument->get_scripts(&spScript);
	if(FAILED(hr))	return;

	CComBSTR bstrMember(strFunc);
	DISPID dispid = 0;
	hr = spScript->GetIDsOfNames(IID_NULL,&bstrMember,1,
		LOCALE_SYSTEM_DEFAULT,&dispid);

	if(FAILED(hr))
	{
		return;
	}

	const int arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs = arraySize;
	dispparams.rgvarg = new VARIANT[dispparams.cArgs];

	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}
	dispparams.cNamedArgs = 0;

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	hr = spScript->Invoke(dispid,IID_NULL,0,
		DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		return;
	}

	if(&pVarResult)
	{
		pVarResult = vaResult;
	}
	return;

}

bool CWebCtrl::ExecuteJScript(LPCTSTR script)
{
	HRESULT hr;

	IWebBrowser2* pBrowser = nullptr;
	IHTMLDocument2* pHtmlDocument = nullptr;
	IHTMLWindow2* pHtmlWindow = nullptr;

	if (!GetHTMLInterface(&pBrowser, &pHtmlDocument, &pHtmlWindow))
	{
		pBrowser->Release();
		pBrowser = nullptr;
		return false;
	}

	USES_CONVERSION;
	BSTR bstrScript = SysAllocString(T2W(script));
	//CString strScript = "function(){if(System!=null&&typeof(System)=='object'&&typeof(System.CloseQuery)=='function') { if(!System.CloseQuery())return; } window.close();}()";
	//CString strScript = "alert('AB');";
	//CString strScript = "if(System!=null&&typeof(System)=='object'&&typeof(System.CloseQuery)=='function') { if(!System.CloseQuery())return; } window.close();";
	//CString strScript = "window.close();";

	VARIANT varResult;

	//pBrowser->ExecWB()
	hr = pHtmlWindow->execScript(CComBSTR(bstrScript), CComBSTR(_T("JScript")), &varResult);
	if (FAILED(hr))
	{
		ATLTRACE(_T("SCRIPT eval fail.hr=0x%08x, scripts=%s\n"), hr, script);
		return false;
	}

	VariantClear(&varResult);
	SysFreeString(bstrScript);
	pHtmlWindow->Release();
	pHtmlWindow = nullptr;
	pHtmlDocument->Release();
	pHtmlDocument = nullptr;
	pBrowser->Release();
	pBrowser = nullptr;
	return true;
}

#ifdef WEBCTRL_AUTOINIT

HWND WINAPI CreateWebControl( LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId )
{
	return CreateWindow( WC_WEBCTRLA, pszText, dwStyles, x, y, cx, cy, hwndParent, (HMENU) uId, GetModuleHandleA( nullptr ), nullptr );
}

HWND WINAPI CreateWebControl( const wchar_t* pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId )
{
	return CreateWindowW( WC_WEBCTRLW, pszText, dwStyles, x, y, cx, cy, hwndParent, (HMENU) uId, GetModuleHandleW( nullptr ), nullptr );
}


static class AutoInit
{
public:

	AutoInit()
	{
		//InitWebCtrl();
	}

	~AutoInit()
	{
		//UninitWebCtrl();
	}

} a;

#endif