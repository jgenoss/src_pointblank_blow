// PropertyEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DockPropertyEditWnd.h"

#include "../PropertyEditor/GlobalVariable.h"
#include "i3Base/string/algorithm/to_lower.h"



#define		DGL_FILTER_I3_REG	"i3Engine Registry File(*.PEF;*.ENV;*.i3Reg;*.i3RegXML;)|*.PEF;*.ENV;*.i3Reg;*.i3RegXML;||"
#define		DLG_FILTER_XML		"i3Engine Registry File(*.XML)|*.XML||"



// cDockAttributeWnd
IMPLEMENT_DYNAMIC(cDockPropertyEditWnd, cDockBaseWnd)

cDockPropertyEditWnd::cDockPropertyEditWnd()
: m_RegCtrl()
, m_CurrentPt(0,0), m_nBarIndex(0)
{

}

cDockPropertyEditWnd::~cDockPropertyEditWnd()
{
	g_pProfile->m_WorkDirectoryPath = g_strRootPath;
	g_pProfile->Save();
	I3_SAFE_DELETE(g_pProfile);
	I3_SAFE_RELEASE(g_pPefList);
	i3RegArray::ReleaseSkipFPBank();
}


BEGIN_MESSAGE_MAP(cDockPropertyEditWnd, cDockBaseWnd)

	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()




int cDockPropertyEditWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( cDockBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	g_HwndDockPropertyWnd = GetSafeHwnd();

	g_pProfile = new CPropEditorProfile;
	g_pPefList	= cPefDataList::new_object();
	g_pPefList->Create();


	//DragAcceptFiles( TRUE);

	CRect rtWnd( 0, 0, 10, 20);

	m_RegCtrl.Create( WS_CHILD | WS_VISIBLE, rtWnd, this, 10000000 );


	m_RegCtrl.SetRoot( g_pPefList->getRoot() );
	g_pPefList->SetRegistryCtrl( &m_RegCtrl );

	SetSelCodePage( i3Language::GetCurrentCodePage());			//	기본 언어로 설정

	return 0;
}



void cDockPropertyEditWnd::OnSize(UINT nType, int cx, int cy)
{
	cDockBaseWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_RegCtrl))
	{
		m_RegCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void cDockPropertyEditWnd::_ReSizeWindow(void)
{
	CRect rect, rect2;
	GetClientRect(rect);

	rect.MoveToY(24);

	rect.right += 4;
	rect.bottom += 3;
	MoveWindow( rect );

	rect.bottom += 1;
	MoveWindow( rect );
}


void cDockPropertyEditWnd::_ReSizeControl(POINT pt, int BarIndex)
{
	if (!g_pProfile)
		return;

	CPoint Length(pt);
	Length = Length - m_CurrentPt;

	switch( BarIndex )
	{
	case 1:
		if( !m_RegCtrl.GetItemWeaponPef() )
		{
			g_pProfile->m_SimpleTree.right += Length.x;
			g_pProfile->m_SimpleList.left += Length.x;		
		}
		else
		{
			g_pProfile->m_Tree1.right += Length.x;
			g_pProfile->m_Tree2.left += Length.x;
			g_pProfile->m_List1.left += Length.x;
		}
		break;
	case 2:
		g_pProfile->m_Tree2.right += Length.x;
		g_pProfile->m_Image.left += Length.x;
		break;
	case 5:
		g_pProfile->m_List1.right += Length.x;
		g_pProfile->m_List2.left += Length.x;
		break;

	case 3:
		g_pProfile->m_Tree2.bottom += Length.y;
		g_pProfile->m_List1.top += Length.y;
		g_pProfile->m_Image.bottom += Length.y;
		g_pProfile->m_List2.top += Length.y;
		break;
	}

	m_CurrentPt = pt;
	g_pProfile->UpdateGapRect();
	m_RegCtrl.ReSizeControl();
}

BOOL cDockPropertyEditWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	static HCURSOR mCurrentCursor = GetCursor();
	static bool bAdjustControlSize = false;

	static HCURSOR mVerCursor = AfxGetApp()->LoadStandardCursor( IDC_SIZEWE );
	static HCURSOR mHorCursor = AfxGetApp()->LoadStandardCursor( IDC_SIZENS );

	int BarIndex = 0;
	POINT Pt = pMsg->pt;
	ScreenToClient( &Pt );
	UINT Result = 0;
	
	if (g_pProfile)
		Result = g_pProfile->MousePointOnSizeBar( Pt, BarIndex );

	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		{
			if( bAdjustControlSize )
				_ReSizeControl(Pt, m_nBarIndex);

			switch( Result )
			{
			case SBT_VER:
				if( mVerCursor != GetCursor() )
					SetCursor( mVerCursor );
				break;
			case SBT_HOR:
				if( mHorCursor != ::GetCursor() )
					SetCursor(mHorCursor);
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if( BarIndex != 0 )
			{
				SetCapture();

				m_nBarIndex = BarIndex;
				bAdjustControlSize = true;
				m_CurrentPt = Pt;
			}
		}
		break;
	case WM_LBUTTONUP: case WM_NCLBUTTONUP:
		{
			if( bAdjustControlSize )
				::ReleaseCapture();
			bAdjustControlSize = false;
		}
		break;
	}

	return cDockBaseWnd::PreTranslateMessage(pMsg);
}



bool cDockPropertyEditWnd::SetPefData(LPCSTR pFilePath, i3RegKey* pRoot)
{
	if ( pFilePath == NULL || pRoot == NULL )
		return false;

	m_RegCtrl.SetItemWeaponPef( false );
	m_RegCtrl.SetRoot( pRoot );

	if (g_pProfile)
		g_pProfile->m_CurrentFilePath	= pFilePath;

	return true;
}

LRESULT cDockPropertyEditWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		I3_TDK_UPDATE_INFO* data = (I3_TDK_UPDATE_INFO*)wParam;
		//char* szLower = (char*)lParam;
		char szClone[256];
		i3::safe_string_copy(szClone,(char*)lParam,256);
		i3::to_lower(szClone);//, strlen(data.item.pszText),ToLower)

		CString strName = (char*)lParam;
		strName.MakeLower();

		CString strSelect = strName;
		CString strClone;

		int nPos = strName.ReverseFind('/');
		if (nPos != -1)
		{
			strSelect	= strName.Mid(0, nPos);
			strClone	= strName.Mid(nPos+1, strName.GetLength());
		}

		if (data->m_Event == I3_TDK_UPDATE_SELECT)
		{
			m_RegCtrl.OnSelectItem(strSelect);
		}
		else if(data->m_Event == I3_TDK_UPDATE_ADD)
		{
			m_RegCtrl.OnCloneItem(strSelect, strClone);
		}
		
	}
	if (message == WM_RBUTTONDOWN)
		return 0;

	return cDockBaseWnd::WindowProc(message, wParam, lParam);
}

void cDockPropertyEditWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	cDockBaseWnd::OnRButtonDown(nFlags, point);
}

void cDockPropertyEditWnd::OnSetFocus(CWnd* pOldWnd)
{
	//cDockBaseWnd::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void	cDockPropertyEditWnd::OnContextMenu( CWnd* pWnd, CPoint /*point*/)
{
}

void cDockPropertyEditWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	cDockBaseWnd::OnRButtonUp(nFlags, point);
}
