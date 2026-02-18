
#include "pch.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;
	
	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(rectDummy, dwStyle, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	m_wndOutputBuild.SetFont(&m_Font);
	m_wndOutputDebug.SetFont(&m_Font);
	m_wndOutputFind.SetFont(&m_Font);


	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);

	  UINT ii;

  //  add a plot to the control.
	 // FLOATPOINT pData2[25];
	 // srand((unsigned)time(NULL));
	 // for (ii=0; ii != 25; ii++)
	 // {
		//pData2[ii].x = (float)(ii % 24);
		//pData2[ii].y = (float)(100000);//(float)((rand() / (RAND_MAX / 20)) - 10.0f);
	 // }
	 // COLORREF crColor2 = RGB (0, 0, 255);
	 // m_wndOutputDebug.Add("Blue Data", crColor2, CLinePlot::LpLine, pData2, 25); 
	
	  for (ii=0; ii != MAX_TIME; ii++)
	  {
		  m_pGraphData[ii].x = (float)(ii % MAX_TIME);
		  m_pGraphData[ii].y = 0.0f;
	  }
	
	  return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_Font);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillWindow( INT32 i32Idx, TCHAR* strText )
{
	switch( i32Idx )
	{
	case 0:
		m_wndOutputBuild.SetCurSel( m_wndOutputBuild.AddString( strText ) );
		break;
	case 1:
		//m_wndOutputDebug.SetCurSel( m_wndOutputDebug.AddString(_T(strText)));
		break;
	}
}

void COutputWnd::UpdateHackCount(UINT32 * pHackCount)
{
/*
	HACK_TYPE_GRAVITY=1,			// position (10.21)
	HACK_TYPE_DAMAGE=2,				// one shot/one kill (2012.01.09 update)
	HACK_TYPE_RANGE=3,				// weapon range(10.21)
	HACK_TYPE_SPEED=4,				// speed hack(10.21)
	HACK_TYPE_WALLSHOT=5,			// wall shot(10.21)
	HACK_TYPE_GRENADE=6,			// max throwing weapon(10.21)
	HACK_TYPE_MAGAZINE=7,			// infinite bullet (=no magazine change)(10.21)
	HACK_TYPE_GHOST=8,				// hide my position (구현 안됨)
	HACK_TYPE_RECALL=9,				// recalls other users (12.24)(명칭변경2012.02.28)
	HACK_TYPE_WEAPON=10,			// weapon change(10.21)
	HACK_TYPE_TRAINING=11,			// A.I.가 안되는 맵 검사(relay 방장 패킷으로 중력핵 검사 방식) // 테스트
	HACK_TYPE_FIRE_SPEED=12,		// shout gun delay hack (2012.01.09)
	HACK_TYPE_INVINCIBILITY=13,		// ignore invincibility time (테스트)
	HACK_TYPE_RESPAWN_TIME=14,		// ignore respawn delay (테스트)
	HACK_TYPE_RESPAWN_POSITION=15,	// respawn position (테스트)
	HACK_TYPE_INSTALL_C4=16,		// install C4 by hack user (테스트)
	HACK_TYPE_CLIENT_TIME=17,		// client time check( speed 관련) (2012.01.09)
*/
	TCHAR strString[1024];
	m_wndOutputFind.ResetContent(); 

	_stprintf_s( strString, _T("GRAVITY			 = %d"), pHackCount[1]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("DAMAGE			 = %d"), pHackCount[2]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("RANGE			 = %d"), pHackCount[3]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("SPEED			 = %d"), pHackCount[4]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("WALLSHOT		 = %d"), pHackCount[5]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("GRENADE			 = %d"), pHackCount[6]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("MAGAZINE		 = %d"), pHackCount[7]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("GHOST			 = %d"), pHackCount[8]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("RECALL			 = %d"), pHackCount[9]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("WEAPON			 = %d"), pHackCount[10]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("TRAINING		 = %d"), pHackCount[11]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("FIRE_SPEED		 = %d"), pHackCount[12]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("INVINCIBILITY	 = %d"), pHackCount[13]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("RESPAWN_TIME	 = %d"), pHackCount[14]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("RESPAWN_POSITION = %d"), pHackCount[15]); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("INSTALL_C4		 = %d"), pHackCount[16]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("CLIENT_TIME		 = %d"), pHackCount[17]);	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("HEADSHOT		 = %d"), pHackCount[18] ); 	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("PACKET			 = %d"), pHackCount[19] );	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );
	_stprintf_s( strString, _T("RECOIL			 = %d"), pHackCount[20] );	m_wndOutputFind.SetCurSel( m_wndOutputFind.AddString( strString ) );

	return; 
}




void COutputWnd::SetGraph(INT32 ccuData)
{
	m_wndOutputDebug.Remove(0);

	for(INT32 i32CurPoint = MAX_TIME; i32CurPoint != 0; i32CurPoint--)
	{
		if(i32CurPoint - 1 != -1)
			m_pGraphData[i32CurPoint].y = m_pGraphData[i32CurPoint - 1].y;
	}

	m_pGraphData[0].y = (float)ccuData;

	COLORREF crColor = RGB (0, 0, 255);
	m_wndOutputDebug.Add( _T("CCU Data"), crColor, CLinePlot::LpLine, m_pGraphData, MAX_TIME);
}


/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("Copy output"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}
