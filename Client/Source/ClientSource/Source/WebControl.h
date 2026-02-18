#ifndef WEBCTRL_H
#define WEBCTRL_H

#define WEBCTRL_AUTOINIT
#include <ocidl.h>
#include <shlobj.h>
#include <Mshtml.h>
#include <MsHtmHst.h>
#include "UI/UIMainFrame.h"
#include "UI/UIPopupBase.h"
#define WC_WEBCTRLA "WebCtrl32"
#define WC_WEBCTRLW L"WebCtrl32"

#ifdef UNICODE
#define WC_WEBCTRL WC_WEBCTRLW
#else
#define WC_WEBCTRL WC_WEBCTRLA 
#endif // UNICODE

#include <ExDispid.h>

class CWebEventSink : public IDispatch
{
	void OnNavigateError( DISPPARAMS *pDispParams);
	void OnDocumentComplete();
	void CloseNewWebbrowser(DISPPARAMS *ppDisp);
	void OpenCurrentWebbrowser(DISPPARAMS *ppDisp);
public:

	CWebEventSink( HWND hwnd)
	{
		m_cRef = 0;
		m_hwnd = hwnd;
	}

	// *** IUnknown ***
	STDMETHOD(QueryInterface)( REFIID riid, PVOID *ppv )
	{
		if ( IsEqualIID( riid, IID_IDispatch ) )
			*ppv = (IDispatch *) this;
		else if ( IsEqualIID( riid, IID_IUnknown ) )
			*ppv = this;
		else
		{
			*ppv = nullptr;
			return E_NOINTERFACE;
		}

		AddRef();

		return S_OK;
	}

	STDMETHOD_(ULONG, AddRef)(void)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"Add RefCount : %d", m_cRef);
		return InterlockedIncrement( &m_cRef );
	}

	STDMETHOD_(ULONG, Release)(void)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"Release RefCount : %d", m_cRef);
		ULONG cRef = InterlockedDecrement( &m_cRef );

		if ( cRef == 0 )
			delete this;

		return cRef;
	}

	// *** IDispatch ***
	STDMETHOD (GetIDsOfNames)( REFIID, OLECHAR **, unsigned int, LCID, DISPID *pdispid )
	{
		*pdispid = DISPID_UNKNOWN;
		return DISP_E_UNKNOWNNAME;
	}

	STDMETHOD (GetTypeInfo)( unsigned int, LCID, ITypeInfo ** )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (GetTypeInfoCount)( unsigned int * )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (Invoke)( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, unsigned int *puArgErr )
	{
		switch( dispIdMember)
		{
		case DISPID_NAVIGATEERROR:
			OnNavigateError( pDispParams);
			break;

		case DISPID_DOCUMENTCOMPLETE:
			OnDocumentComplete();
			break;
		case DISPID_NEWWINDOW2:
			CloseNewWebbrowser(pDispParams);
			break;
		case DISPID_NEWWINDOW3:
			OpenCurrentWebbrowser(pDispParams);
			break;
		}
		return S_OK;
	}
private:

	LONG m_cRef;
	HWND m_hwnd;
};

class CWebContainer : //public IOleContainer,
	public IOleClientSite, 
	public IOleInPlaceSite,
	public IOleInPlaceFrame,
	public IOleControlSite,
	public IOleCommandTarget,
	public IDispatch
{
public:

	CWebContainer( HWND );
	~CWebContainer();

	// *** IUnknown ***
	STDMETHOD(QueryInterface)( REFIID riid, PVOID *ppv )
	{
		if ( IsEqualIID( riid, IID_IOleClientSite ) )
			*ppv = (IOleClientSite *) this;
		else if ( IsEqualIID( riid, IID_IOleContainer))
			*ppv = (IOleContainer *) this;
		else if ( IsEqualIID( riid, IID_IOleInPlaceSite ) )
			*ppv = (IOleInPlaceSite *) this;
		else if ( IsEqualIID( riid, IID_IOleInPlaceFrame ) )
			*ppv = (IOleInPlaceFrame *) this;
		else if ( IsEqualIID( riid, IID_IOleInPlaceUIWindow ) )
			*ppv = ( IOleInPlaceUIWindow *) this;
		else if ( IsEqualIID( riid, IID_IOleControlSite ) )
			*ppv = (IOleControlSite *)this;
		else if ( IsEqualIID( riid, IID_IOleWindow ) )
			*ppv = this;
		else if ( IsEqualIID( riid, IID_IOleCommandTarget) )
			*ppv = (IOleCommandTarget *)this;
		else if ( IsEqualIID( riid, IID_IDispatch ) )
			*ppv = (IDispatch *) this;
		else if ( IsEqualIID( riid, IID_IUnknown ) )
			*ppv = this;
		else
		{
			*ppv = nullptr;
			return E_NOINTERFACE;
		}

		AddRef();

		return S_OK;
	}

	STDMETHOD_(ULONG, AddRef)(void)
	{
		//I3PRINTLOG(I3LOG_NOTICE,"Add RefCount : %d", m_cRef);
		return InterlockedIncrement( &m_cRef );
	}

	STDMETHOD_(ULONG, Release)(void)
	{
		ULONG cRef = InterlockedDecrement( &m_cRef );
		//I3PRINTLOG(I3LOG_NOTICE,"Release RefCount : %d", m_cRef);
		if ( cRef == 0 )
			delete this;

		return cRef;
	}

	// *** IOleClientSite ***
	STDMETHOD (SaveObject)()
	{
		return E_NOTIMPL;
	}

	STDMETHOD (GetMoniker)( DWORD, DWORD, LPMONIKER * )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (GetContainer)( LPOLECONTAINER * )
	{
		return E_NOINTERFACE;
	}

	STDMETHOD (ShowObject)()
	{
		return S_OK;
	}

	STDMETHOD (OnShowWindow)( BOOL )
	{
		return S_OK;
	}

	STDMETHOD (RequestNewObjectLayout)()
	{
		return E_NOTIMPL;
	}

	// *** IOleWindow ***
	STDMETHOD (GetWindow)( HWND *phwnd )
	{
		*phwnd = m_hwnd;
		return S_OK;
	}

	STDMETHOD (ContextSensitiveHelp)( BOOL )
	{
		return E_NOTIMPL;
	}

	// *** IOleCommandTarget ***
	STDMETHOD (QueryStatus)( const GUID *, ULONG, OLECMD[], OLECMDTEXT *)
	{
		return S_OK;
	}

	STDMETHOD (Exec)(const GUID *pguidCmdGroup,DWORD nCmdID,DWORD nCmdexecopt,VARIANT* pvaIn,VARIANT* pvaOut)
	{

		HRESULT hr = S_OK;

		if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
		{
			switch (nCmdID) 
			{
			case OLECMDID_PAGEACTIONBLOCKED:
				{
					break;
				}
			case OLECMDID_SHOWSCRIPTERROR:
				{
					(*pvaOut).vt = VT_BOOL;
					(*pvaOut).boolVal = VARIANT_TRUE;
					return S_OK;
				}
			case OLECMDID_SHOWMESSAGE:
				{
					(*pvaOut).boolVal = VARIANT_TRUE;
				}
			default:
				hr = OLECMDERR_E_NOTSUPPORTED;
				break;
			}
		}
		else
		{
			hr = OLECMDERR_E_UNKNOWNGROUP;
		}
		return (hr);
	}

	// *** IOleInPlaceSite ***
	STDMETHOD (CanInPlaceActivate)(void)
	{
		return S_OK;
	}

	STDMETHOD (OnInPlaceActivate) (void)
	{
		return S_OK;
	}

	STDMETHOD (OnUIActivate) (void)
	{
		return S_OK;
	}

	STDMETHOD (GetWindowContext)(
		IOleInPlaceFrame    **ppFrame, 
		IOleInPlaceUIWindow **ppIIPUIWin,
		LPRECT                prcPosRect,
		LPRECT                prcClipRect,
		LPOLEINPLACEFRAMEINFO pFrameInfo )
	{
		*ppFrame = (IOleInPlaceFrame *) this;
		*ppIIPUIWin = nullptr;

		RECT rc;

		GetClientRect( m_hwnd, &rc );

		prcPosRect->left   = 0;
		prcPosRect->top    = 0;
		prcPosRect->right  = rc.right;
		prcPosRect->bottom = rc.bottom;

		CopyRect( prcClipRect, prcPosRect );

		pFrameInfo->cb             = sizeof(OLEINPLACEFRAMEINFO);
		pFrameInfo->fMDIApp        = false;
		pFrameInfo->hwndFrame      = m_hwnd;
		pFrameInfo->haccel         = nullptr;
		pFrameInfo->cAccelEntries  = 0;

		(*ppFrame)->AddRef();

		return S_OK;
	}

	STDMETHOD (Scroll)( SIZE )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (OnUIDeactivate)( BOOL )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (OnInPlaceDeactivate)(void)
	{
		return S_OK;
	}

	STDMETHOD (DiscardUndoState)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD (DeactivateAndUndo)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD (OnPosRectChange)( LPCRECT )
	{
		return S_OK;
	}

	// *** IOleInPlaceUIWindow ***
	STDMETHOD (GetBorder)( LPRECT )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (RequestBorderSpace)( LPCBORDERWIDTHS )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (SetBorderSpace)( LPCBORDERWIDTHS )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (SetActiveObject)( IOleInPlaceActiveObject *, LPCOLESTR )
	{
		return E_NOTIMPL;
	}

	// *** IOleInPlaceFrame ***
	STDMETHOD (InsertMenus)( HMENU, LPOLEMENUGROUPWIDTHS )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (SetMenu)( HMENU, HOLEMENU, HWND )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (RemoveMenus)( HMENU )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (SetStatusText)( LPCOLESTR )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (EnableModeless)( BOOL )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (TranslateAccelerator)( LPMSG, WORD )
	{
		return S_OK;
	}

	// *** IOleControlSite ***
	STDMETHOD (OnControlInfoChanged)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD (LockInPlaceActive)( BOOL )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (GetExtendedControl)( IDispatch ** )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (TransformCoords)( POINTL *, POINTF *, DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (TranslateAccelerator)( LPMSG, DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (OnFocus)( BOOL )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (ShowPropertyFrame)(void)
	{
		return E_NOTIMPL;
	}

	// *** IDispatch ***
	STDMETHOD (GetIDsOfNames)( REFIID, OLECHAR **, unsigned int, LCID, DISPID *pdispid )
	{
		*pdispid = DISPID_UNKNOWN;
		return DISP_E_UNKNOWNNAME;
	}

	STDMETHOD (GetTypeInfo)( unsigned int, LCID, ITypeInfo ** )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (GetTypeInfoCount)( unsigned int * )
	{
		return E_NOTIMPL;
	}

	STDMETHOD (Invoke)( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, unsigned int *puArgErr )
	{
		//COPYDATASTRUCT cpd; 
		UIMainFrame * pFrame = nullptr;
		switch( dispIdMember)
		{
		case DISPID_WINDOWCLOSING:
			pFrame = g_pFramework->GetUIMainframe();
			pFrame->ClosePopup(UPW_WEB_BOX);
			//::PostMessage(m_hwnd, WM_DESTROY, NULL, NULL);
			break;
		case DISPID_ONQUIT:
			{
				//char *str=nullptr;
				//  IWebBrowser2 *pweb;
				//  LPOLESTR lpURL = nullptr;

				//  m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) pweb );
				//  pweb->get_LocationURL(&lpURL);   
				//  str = "77777";   
				//  if(!strstr(str, "://"))   
				//   break;   
				//  //hwnd=::FindWindow(NULL,"貢쭹輦籃");   
				//  if(m_hwnd)     
				//  {   
				//   cpd.dwData = IE_ONQUIT;   
				//   cpd.cbData = strlen(str) + 1;   
				//   cpd.lpData = (void*)str;   
				::SendMessage(m_hwnd, WM_QUIT, NULL, NULL);
				//}
				break;
			}
		case DISPID_QUIT:
			//SendMessage(m_hwnd, WM_QUIT, )
			//OnDocumentComplete();
			::SendMessage(m_hwnd, WM_QUIT, NULL, NULL);
			break;

		case DISPID_NAVIGATEERROR:		
		case DISPID_DOCUMENTCOMPLETE:
		case DISPID_NEWWINDOW2:
		case DISPID_NEWWINDOW3:

			m_pEvent->Invoke( dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
			break;
		}
		return DISP_E_MEMBERNOTFOUND;
	}

	void setLocation( int, int, int, int );
	void setVisible( bool );
	void setFocus( bool );
	void add();
	void remove();
	HRESULT getWeb( IWebBrowser2 ** );

private:

	void ConnectEvents();
	void DisconnectEvents();

	IConnectionPoint *GetConnectionPoint( REFIID );

	LONG           m_cRef;
	HWND           m_hwnd;
	IUnknown      *m_punk;
	RECT           m_rect;
	CWebEventSink *m_pEvent;
	DWORD          m_eventCookie;
	void*		  m_pParent;
};

class CWebCtrl;
typedef void (*cb_webevent)( CWebCtrl*);

class CWebCtrl
{
public:

	CWebCtrl( HWND );
	BOOL GetHTMLInterface(IWebBrowser2** ppBrowser, IHTMLDocument2** ppHtmlDocument, IHTMLWindow2** ppHtmlWindow);
	bool ExecuteJScript(LPCTSTR script);
	void ExecuteJScript2(const CString strFunc, const CStringArray& paramArray);
	bool ExecuteJScript3(const CString strFunc, const CStringArray& paramArray);
	LPDISPATCH GetHtmlDocument() const;

	HRESULT GethrNavigate2() const			{ return m_hrNavigate2;}
	DWORD GetNavigate2ErrorCode() const		{ return m_dwErrorCode;}
	void SethrNavigate2( HRESULT hr) { m_hrNavigate2 = hr;}
	void SethrNavigate2ErrorCode( DWORD error) { m_dwErrorCode = error;}

	void SetCBDocumentComplete( cb_webevent cb) { cb_docmuent_complete = cb;}
	void OnDocumentComplete() { if( cb_docmuent_complete) cb_docmuent_complete( this); cb_docmuent_complete = 0;}
	IWebBrowser2* GetWebBrowser2() {return m_pweb;}
	
	bool isCloseNewWebbrowser()				{ return m_bCloseNewWebbrowser;		}
	bool isOpenSteamWebbrowser()			{ return m_bSteamWebbrowser;		}
	bool isPBWebboxuibrowser()				{ return m_bPBWebboxuibrowser;		}

	void setCloseNewWebbrowser(bool onoff)	{ m_bCloseNewWebbrowser = onoff;	}
	void setSteamWebbrowser(bool onoff)		{ m_bSteamWebbrowser = onoff;		}
	void setPBWebboxuibrowser(bool onoff)	{ m_bPBWebboxuibrowser = onoff;		}
	 
private:

	friend LRESULT CALLBACK WebCtrlProc( HWND, UINT, WPARAM, LPARAM );

	LRESULT WebCtrlProc( UINT, WPARAM, LPARAM );

	void Navigate( LPTSTR );

	HWND m_hwnd;

	int  m_cxScroll;
	int  m_cyScroll;
	int	x,y;
	
	bool m_bCloseNewWebbrowser;		//다른 링크를 클릭했을때 무조건 현재 browser에 web을 열게함
	bool m_bSteamWebbrowser;		//다른 링크를 클릭했을때 Steam browser에 web을 열게함
	bool m_bPBWebboxuibrowser;		//다른 링크를 클릭했을때 UIPopupWebbox에 web을 열게함
	
	CWebContainer *m_pContainer;

	IWebBrowser2 *m_pweb;

	HRESULT m_hrNavigate2;
	DWORD m_dwErrorCode;

	cb_webevent cb_docmuent_complete;
};

bool WINAPI InitWebCtrl();
bool WINAPI UninitWebCtrl();

#ifdef WEBCTRL_AUTOINIT

HWND WINAPI CreateWebControl( LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId );
HWND WINAPI CreateWebControl( const wchar_t* pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId );

#endif // WEBCTRL_AUTOINIT

#endif // WEBCTRL_H
