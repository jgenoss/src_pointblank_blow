// CUIPaneScriptList
#include "stdafx.h"
#include "../i3UIEditor.h"
#include "UIPaneScriptList.h"
#include "../MainFrm.h"

IMPLEMENT_DYNAMIC(CUIPaneScriptList, CUIPaneBase)

CUIPaneScriptList::CUIPaneScriptList()
{
}

CUIPaneScriptList::~CUIPaneScriptList()
{
}

BEGIN_MESSAGE_MAP(CUIPaneScriptList, CUIPaneBase)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_LBN_DBLCLK( ID_PANE_SCRIPT_LISTBOX, OnLBDblClicked)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CUIPaneScriptList message handlers
int CUIPaneScriptList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rc;

		GetClientRect( &rc);

		BOOL bResult = m_ScriptList.Create( WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY, rc, this, ID_PANE_SCRIPT_LISTBOX);
		
		I3ASSERT( bResult);
	}

	return 0;
}

void CUIPaneScriptList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDockablePane::OnPaint() for painting messages
	RECT rt;
	::GetClientRect( m_hWnd, &rt);

	::FillRect( dc.m_ps.hdc,&rt, (HBRUSH)COLOR_WINDOW);
}

void CUIPaneScriptList::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ScriptList.m_hWnd))
	{
		m_ScriptList.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CUIPaneScriptList::OnLBDblClicked( void)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	I3ASSERT( pMainFrm != NULL);

	/*char szScript[MAX_PATH];
	m_ScriptList.GetText( m_ScriptList.GetCurSel(), szScript);

	bool bFile = _isFile( szScript);
	bool bScreen = _isScreen( szScript);

	pMainFrm->OnEditScript( szScript, bFile, bScreen);*/
}

void CUIPaneScriptList::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	//StringList strList;
	//m_ScriptList.ResetContent();

	//pScriptManager->GetAllBlocknames( &strList);		//AddStrings

	//	// UIScene에 설정된 파일
	//	char conv[MAX_PATH];
	//	pScriptManager->GetSimplename( conv);

	//	if( conv[0] != 0)
	//		m_ScriptList.AddString( conv);
	//}
	//
	//for( INT32 i=0; i<strList.GetCount(); i++)
	//{
	//	m_ScriptList.AddString( (char*)strList.GetItem(i));
	//}	
	

	// NReplace Test
	//char p1[20];
	//char p2[20];
	//char p3[10];
	//
	//i3::string_ncopy_nullpad( p1, "1111 2222 33333", sizeof( p1));
	//i3::string_ncopy_nullpad( p2, "You are a girl", sizeof( p2));
	//i3::string_ncopy_nullpad( p3, "ABCDEFG", sizeof( p3));

	//i3String::NReplace( p1, 0, 1, sizeof(p1), "Go", 0);
	//i3String::NReplace( p2, 0, 1, 100, p1, i3::generic_string_size(p1));
	//i3String::NReplace( p3, 3, 3, sizeof(p3), "z", 1);

	//char c1[6];
	//i3::string_ncopy_nullpad( c1, "abcde", sizeof( c1));
	//bool bRe = i3String::NReplace( c1, 0, 4, sizeof( c1), "ABCDEF", 3);

	//I3TRACE("%s\n%s\n%s%d\n", p1, p2, p3, bRe);
}