// CScriptPane.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptPane.h"
#include "MainFrm.h"

#include "i3Base/string/ext/extract_filename.h"
// CScriptPane

IMPLEMENT_DYNAMIC(CScriptPane, CDockablePane)

CScriptPane::CScriptPane()
{
	m_bScreen = false;
}

CScriptPane::~CScriptPane()
{
}


BEGIN_MESSAGE_MAP(CScriptPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int CScriptPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	CRect rc;
	GetClientRect( rc);
	
	HMODULE hmod = LoadLibrary( "SciLexer.dll");

	if( hmod == NULL)
	{
	   MessageBox( "The Scintilla DLL could not be loaded.", "Error loading Scintilla", MB_OK | MB_ICONERROR);
	}

	BOOL bRes = m_ScriptWnd.CreateEx( 0, "Scintilla", "AAA", WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN, rc, this, 0);

	if( bRes)
		m_ScriptWnd.Initialize();

	return 0;
}

void CScriptPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_ScriptWnd.m_hWnd))
	{
		CRect rc;
		GetClientRect(rc);
		
		m_ScriptWnd.SetWindowPos( NULL, rc.left, rc.top, rc.right, rc.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


//=============================================================================================
void CScriptPane::New( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.New();
}

const char*	CScriptPane::GetFilename(void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	return m_ScriptWnd.GetFilename();
}

void CScriptPane::GetSimplename( char* buf)
{
	buf[0] = 0;

	const char* pszFullName = GetFilename();

	if( pszFullName != NULL)
	{
//		i3String::SplitFileName( pszFullName, buf, FALSE);
		i3::extract_filetitle(pszFullName, buf);
	}
}

void CScriptPane::SetFilename( const char* filename)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.SetFilename( filename);
}

void CScriptPane::OpenFile( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.OpenFile();
}

void CScriptPane::SaveFile( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.SaveFile();
}

void CScriptPane::OpenBlock( const char* blockname)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.OpenBlock( blockname);
}

void CScriptPane::SaveBlock( const char* blockname)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.SaveBlock( blockname);
}

void CScriptPane::RenameBlock( const char* oldname, const char* newname)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.RenameBlock( oldname, newname);
}

void CScriptPane::Save( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.Save();
}

void CScriptPane::ReOpen( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.ReOpen();
}

bool CScriptPane::isFileOpened( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	return m_ScriptWnd.isFileOpened();
}

bool CScriptPane::SaveIfModified( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	return m_ScriptWnd.SaveIfModified();
}

bool CScriptPane::IsModified( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	return m_ScriptWnd.IsModified();
}

void CScriptPane::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	
	// OnCloseMiniFrame
	// OnCloseDockingPane
	// OnPressCloseButton
	// 저 세군데만 잘 처리해주면 된다. PostMessage
	
	ShowWindow( SW_HIDE); 

	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->RecalcLayout(); 
	pMainFrm->OnCloseScript( this);

	CDockablePane::OnClose();
}

void CScriptPane::OnDestroy()
{
	CDockablePane::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL CScriptPane::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CDockablePane::DestroyWindow();
}

void CScriptPane::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	m_ScriptWnd.SetFocus();
}

void CScriptPane::OnPressCloseButton()
{
	I3TRACE("CScriptPane::OnPressCloseButton\n");

	// OnCloseDockingPane, OnCloseMiniFrame 에 들어오고나서 또 여기 들어오는 경우가 있네..
	// 저기서 PostMessage(WM_CLOSE)를 보냈다면 Pane이 이미 닫힌상태로 여기 들어왔을 수도 있다.
	
	/*bool bClose = SaveIfModified();

	if( bClose)
		PostMessage(WM_CLOSE); */
}

void CScriptPane::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here

	//if(!bShow && !IsWindowVisible() && IsInFloatingMultiPaneFrameWnd())
	if( !bShow && !IsWindowVisible())
		I3TRACE("CLOSED!!!\n");
}

void CScriptPane::setNeedToReload( void)
{
	I3ASSERT( ::IsWindow( m_ScriptWnd.m_hWnd));
	m_ScriptWnd.setNeedToReload();	
}

void CScriptPane::OnSavingScript( void)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	pMainFrm->OnSavingScript( this);
}