// i3TDKProfilingViewerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKProfilingViewerCtrl.h"
#include "i3TDKPropertySubItemStatic.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/atoi.h"

#if !defined( I3_NO_PROFILE)

#define ID_TIMER_USAGE_HISTORY_REDRAW		4000
#define ID_UPDATE_TIME_EDIT					4001
#define ID_BTN_APPLY						4002

#define SCROLL_PIXEL_COUNT				5
#define VIEWER_CTRL_PIXEL_GAP			20
#define	HISTORY_GRID_PIXEL_GAP			16

#define REAL_PROFILE_CHILD_EXPAND		I3_REAL_PROFILE_USER_FLAG

// i3TDKProfilingViewerCtrl

IMPLEMENT_DYNAMIC(i3TDKProfilingViewerCtrl, CWnd)

i3TDKProfilingViewerCtrl::~i3TDKProfilingViewerCtrl()
{
	_DeleteAllProfilingNode();
	_DeleteAllProfilingList();

	::SelectObject( m_hDrawDC, m_hbmpOld);
	::DeleteObject( m_hbmpDraw);

	::DeleteDC( m_hDrawDC);
	m_hDrawDC = nullptr;

	KillTimer( ID_TIMER_USAGE_HISTORY_REDRAW);

	m_CaptionFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(i3TDKProfilingViewerCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_NOTIFY( NM_CLICK, PROFILING_TREE, OnNMLClickTree)
END_MESSAGE_MAP()



// i3TDKProfilingViewerCtrl message handlers
BOOL i3TDKProfilingViewerCtrl::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR	hCursor;
	HBRUSH	hBack;

	m_dwStyle = dwStyle;

	m_TimerSec = 0;
	m_pRoot = nullptr;
	m_pCurNode = nullptr;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH)GetStockObject( WHITE_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	return CWnd::Create(pszClass, _T(""), dwStyle | WS_CLIPCHILDREN, rect, pParentWnd, nID, nullptr);
}

/*BOOL i3TDKProfilingViewerCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}*/

void i3TDKProfilingViewerCtrl::SetUpdateTime( UINT32 timeElapse)
{	
	m_nTimerElapse = timeElapse;	//	0일 경우 매프레임 업데이트 한다. (default)

	if( m_hTimer != nullptr)
	{
		KillTimer( ID_TIMER_USAGE_HISTORY_REDRAW);
	}

	m_hTimer = (HANDLE) SetTimer( ID_TIMER_USAGE_HISTORY_REDRAW, m_nTimerElapse, 0); 

	TCHAR szVal[256];
	i3::snprintf( szVal, _countof(szVal), _T("%d"), m_nTimerElapse);
	m_UpdateTimeEdit.SetWindowText( szVal);
}

void i3TDKProfilingViewerCtrl::InitMain( void)
{
	m_bCreate = FALSE;

	//	Create font
	VERIFY( m_CaptionFont.CreateFont(
									12,                        // nHeight
									0,                         // nWidth
									0,                         // nEscapement
									0,                         // nOrientation
									FW_NORMAL,                 // nWeight
									FALSE,                     // bItalic
									FALSE,                     // bUnderline
									0,                         // cStrikeOut
									ANSI_CHARSET,              // nCharSet
									OUT_DEFAULT_PRECIS,        // nOutPrecision
									CLIP_DEFAULT_PRECIS,       // nClipPrecision
									DEFAULT_QUALITY,           // nQuality
									DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
									_T("Tahoma") ));                 // lpszFacename
		
	//	Create Ctrl
	InitProfilingTree();
	InitProfilingList();
	InitProfilingHistory();
	
	{
		//	UpdateTime Edit
		TCHAR tmp[256];
		RECT rect;
		GetClientRect( &rect);

		rect.left = rect.right - 110;
		rect.top = rect.bottom - 35;
		rect.right -= 70;
		rect.bottom -= 15;

		m_UpdateTimeEdit.Create( WS_CHILD | WS_BORDER | WS_VISIBLE | WS_EX_STATICEDGE, rect, this, ID_UPDATE_TIME_EDIT);
		m_UpdateTimeEdit.SetFont( &m_CaptionFont);
		m_UpdateTimeEdit.SetLimitText( 256);

		i3::snprintf( tmp, _countof(tmp), _T("%d"), m_nTimerElapse);
		m_UpdateTimeEdit.SetWindowText( tmp);

		//	Apply Button
		GetClientRect( &rect);

		rect.left = rect.right - 65;
		rect.top = rect.bottom - 35;
		rect.right -= 20;
		rect.bottom -= 15;
		
		m_BtnApply.Create( _T("Apply"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect, this, ID_BTN_APPLY);
		m_BtnApply.SetFont( &m_CaptionFont);
	}

	m_bCreate = TRUE;

	//	Timer를 사용하려면 SetUpdateTime()을 호출해야 합니다.
	//m_hTimer = (HANDLE) SetTimer( ID_TIMER_USAGE_HISTORY_REDRAW, m_nTimerElapse, 0); 
}

void i3TDKProfilingViewerCtrl::InitProfilingTree( void)
{
	//	Profiling Node Tree
	{
		CRect rect;
		GetClientRect( rect);

		rect.left += VIEWER_CTRL_PIXEL_GAP;
		rect.top += VIEWER_CTRL_PIXEL_GAP;
		rect.right = rect.Width() / 3;
		rect.bottom -= VIEWER_CTRL_PIXEL_GAP;
		
		BOOL ret = m_ProfilingTreeCtrl.Create( m_dwStyle | WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | 
												TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP,
												rect, this, PROFILING_TREE);
		I3ASSERT( ret == TRUE);
	}
}

void i3TDKProfilingViewerCtrl::InitProfilingList( void)
{
	//	Profiling Usage list
	{
		CRect rect;
		GetClientRect( rect);

		rect.left = (rect.Width() / 3) + VIEWER_CTRL_PIXEL_GAP;
		rect.top += VIEWER_CTRL_PIXEL_GAP;
		rect.right -= VIEWER_CTRL_PIXEL_GAP;
		rect.bottom = (INT32) ((REAL32) rect.Height() * 0.5f);
		BOOL ret = m_ProfilingListCtrl.Create( m_dwStyle | WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | WS_VISIBLE | WS_BORDER | 
												WS_HSCROLL | WS_VSCROLL | LVS_SHOWSELALWAYS | LVS_REPORT,
												rect, this, PROFILING_LIST);
		I3ASSERT( ret == TRUE);
	}
}

void i3TDKProfilingViewerCtrl::InitProfilingHistory( void)
{
	//	Profiling Usage History
	{	
		LPCTSTR pszClass;
		HCURSOR hCursor;
		HBRUSH	hbrBack;

		CRect rect;
		GetClientRect( rect);

		hCursor = LoadCursor( nullptr, IDC_ARROW);
		hbrBack = (HBRUSH) GetStockObject( WHITE_BRUSH);

		pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_OWNDC, hCursor, hbrBack, nullptr);
		//pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hbrBack, nullptr);

		rect.left = (rect.Width() / 3) + VIEWER_CTRL_PIXEL_GAP;
		rect.top = (INT32) ((REAL32) rect.Height() * 0.5f) + VIEWER_CTRL_PIXEL_GAP;
		rect.right -= VIEWER_CTRL_PIXEL_GAP;
		rect.bottom -= VIEWER_CTRL_PIXEL_GAP + 20;
		
		BOOL ret = m_ProfilingHistory.Create( pszClass, _T("Usage History"), 
												m_dwStyle | WS_CLIPCHILDREN | WS_CHILD | WS_BORDER | WS_VISIBLE, 
												rect, this, PROFILING_VIEW);
		I3ASSERT( ret == TRUE);

		//	DC
		HWND hwnd = m_ProfilingHistory.GetSafeHwnd();
		HDC dc = ::GetDC( hwnd);

		m_hDrawDC = CreateCompatibleDC( dc);
		I3ASSERT( m_hDrawDC != nullptr);

		::ReleaseDC( hwnd, dc);

		::GetClientRect( hwnd, rect);

		m_nDrawWidth = rect.Width();
		m_nDrawHeight = rect.Height();

		m_nScrollPixel = (m_nDrawWidth / m_nWidthGridCount) / 2;

		::SetMapMode( m_hDrawDC, MM_TEXT);
		::SetWindowExtEx( m_hDrawDC, m_nDrawWidth, m_nDrawHeight, 0);
		::SetViewportExtEx( m_hDrawDC, m_nDrawWidth, m_nDrawHeight, 0);

		//	Create drawing bitmap
		m_hbmpDraw = CreateBitmap( m_nDrawWidth, m_nDrawHeight, 
									GetDeviceCaps( m_hDrawDC, PLANES),
									GetDeviceCaps( m_hDrawDC, BITSPIXEL), nullptr);
		I3ASSERT( m_hbmpDraw != nullptr);

		m_hbmpOld = (HBITMAP) ::SelectObject( m_hDrawDC, m_hbmpDraw);
	}
}

void i3TDKProfilingViewerCtrl::_MakeProfilingTree( REAL_PROFILE_NODE * pRoot)
{
	__CHECKF( m_bCreate != FALSE, _T("failed! Make tree is not ready") );

	//__CHECKW( pRoot->GetParent() != nullptr, "failed! Node is not root");

	m_pRoot = m_pCurNode = pRoot;

	_DeleteAllProfilingNode();

	_AddProfilingNode( m_pRoot, nullptr);
	I3ASSERT( m_hRootItem != nullptr);

	//m_ProfilingTreeCtrl.EnsureVisible( m_hRootItem);
}

INT32 i3TDKProfilingViewerCtrl::_AddProfilingNode( REAL_PROFILE_NODE * pNode, HTREEITEM hParent)
{
	__CHECKW_RT( pNode != nullptr, _T("failed! Add Node is null"), 0);

	//TVINSERTSTRUCT item;
	HTREEITEM hNewItem = nullptr;
	INT32 count = 0;
	
#ifdef _UNICODE
	i3::stack_wstring wstrNodeName;	i3::mb_to_utf16(pNode->GetName(), wstrNodeName);
	const wchar_t* szNodeName = wstrNodeName.c_str();
#else
	const char* szNodeName = pNode->GetName();
#endif

	if( hParent == nullptr)	//	Root
	{
		hNewItem = m_hRootItem = m_ProfilingTreeCtrl.InsertItem( szNodeName, TVI_ROOT, TVI_LAST);
	}
	else
	{
		hNewItem = m_ProfilingTreeCtrl.InsertItem( szNodeName, hParent, TVI_LAST);
	}

	__CHECKW_RT( hNewItem != nullptr, _T("failed! Insert node in tree ctrl."), 0);

	pNode->AddStyle( REAL_PROFILE_CHILD_EXPAND);

	m_ProfilingTreeCtrl.SetItemData( hNewItem, (DWORD_PTR) pNode);	

	count++;

	//	자식 노드 추가
	for( INT32 i = 0; i < pNode->GetChildCount(); i++)
	{
		count += _AddProfilingNode( pNode->GetChild( i), hNewItem);
	}	

	m_ProfilingTreeCtrl.EnsureVisible( hNewItem);

	return count;
}


void i3TDKProfilingViewerCtrl::_DeleteAllProfilingNode( void)
{
	m_ProfilingTreeCtrl.DeleteAllItems();
}

void i3TDKProfilingViewerCtrl::_MakeProfilingList( REAL_PROFILE_NODE * pRoot)
{
	_DeleteAllProfilingList();


	m_ListNodeArray.clear();

	
	//	Add column
	m_ProfilingListCtrl.AddColumnTitle( _T("Name"), 40, LVCFMT_CENTER);
	m_ProfilingListCtrl.AddColumnTitle( _T("%parent"), 17, LVCFMT_CENTER);
	m_ProfilingListCtrl.AddColumnTitle( _T("%total"), 17, LVCFMT_CENTER);
	m_ProfilingListCtrl.AddColumnTitle( _T("calc"), 16, LVCFMT_CENTER);
	m_ProfilingListCtrl.AddColumnTitle( _T("call"), 10, LVCFMT_CENTER);		
	
	//_AddProfilingList( 0, pRoot);

	{
		

		//	ListCtrl에 Item을 넣기전에 미리 List로 구성한다.
		_MakeProfilingListNode( pRoot);

		const size_t nNodeCount = m_ListNodeArray.size();

		//	ListCtrl에 출력할 Item을 넣는다.
		if( nNodeCount > 0)
		{
			m_ProfilingListCtrl.NewSubItem( (INT32) nNodeCount);

			for( size_t i = 0; i < nNodeCount; i++)
			{
				REAL_PROFILE_NODE *			pNode = m_ListNodeArray[i];
				I3ASSERT( pNode != nullptr);

				REAL_PROFILE_HISTORY *		pHistory = pNode->GetHistory();
				I3ASSERT( pHistory != nullptr);

				TCHAR szlabel[256];

				//	Name
				{
					SubItemStaticInitStruct initSt;					

					REAL_PROFILE_NODE * pParent = pNode->GetParent();

					szlabel[0] = '\0';
					while( pParent != nullptr)
					{
						i3::generic_string_cat( szlabel, _T(" ") );

						pParent = pParent->GetParent();
					}

					if( pNode->GetParent() != nullptr)
					{	
						i3::generic_string_cat( szlabel, _T("- ") );
					}
#ifdef _UNICODE
					i3::stack_wstring wstrHistoryName;	i3::mb_to_utf16(pHistory->m_pszName, wstrHistoryName);
					i3::generic_string_cat(szlabel, wstrHistoryName);
#else
					i3::generic_string_cat( szlabel, pHistory->m_pszName);
#endif
					initSt._label = szlabel;
					initSt._column = 0;
					initSt._row = i;
					initSt._font_color = RGB( 50, 50, 150);

					if( m_ProfilingListCtrl.SetSubItemStatic( &initSt) == nullptr)
					{
						__CHECKW( 0, _T("Add Item List is fail") );
					}
				}

				//	Usage
				{
					SubItemStaticInitStruct initSt;

					if( pNode->GetParent() == nullptr)
					{
						i3::snprintf( szlabel, _countof(szlabel), _T("-- ") );
					}
					else
					{
						i3::snprintf( szlabel, _countof(szlabel), _T("%.2f "), pHistory->m_fUsageRate);
					}
					
					initSt._label = szlabel;
					initSt._column = 1;
					initSt._row = i;
					initSt._style = SUBITEM_STYLE_TEXTALIGN_RIGHT;

					if( m_ProfilingListCtrl.SetSubItemStatic( &initSt) == nullptr)
					{
						__CHECKW( 0, _T("Add Item List is fail") );
					}
				}

				//	Total Usage
				{
					SubItemStaticInitStruct initSt;
					
					i3::snprintf( szlabel, _countof(szlabel), _T("%.2f "), pHistory->m_fTotalUsageRate);
					initSt._label = szlabel;
					initSt._column = 2;
					initSt._row = i;
					initSt._style = SUBITEM_STYLE_TEXTALIGN_RIGHT;

					if( m_ProfilingListCtrl.SetSubItemStatic( &initSt) == nullptr)
					{
						__CHECKW( 0, _T("Add Item List is fail") );
					}
				}

				//	Profile Time
				{
					SubItemStaticInitStruct initSt;
					
					i3::snprintf( szlabel, _countof(szlabel), _T("%.4f "), pHistory->m_fProfileTime);
					initSt._label = szlabel;
					initSt._column = 3;
					initSt._row = i;
					initSt._style = SUBITEM_STYLE_TEXTALIGN_RIGHT;

					if( m_ProfilingListCtrl.SetSubItemStatic( &initSt) == nullptr)
					{
						__CHECKW( 0, _T("Add Item List is fail") );
					}
				}

				//	Call Count
				{
					SubItemStaticInitStruct initSt;

					i3::snprintf( szlabel, _countof(szlabel), _T("%d "), pHistory->m_TotalCalls);
					initSt._label = szlabel;
					initSt._column = 4;
					initSt._row = i;	
					initSt._style = SUBITEM_STYLE_TEXTALIGN_RIGHT;

					if( m_ProfilingListCtrl.SetSubItemStatic( &initSt) == nullptr)
					{
						__CHECKW( 0, _T("Add Item List is fail") );
					}
				}
			}
		}
	}
}

void i3TDKProfilingViewerCtrl::_MakeProfilingListNode( REAL_PROFILE_NODE * pNode)
{
	if( pNode != nullptr)
	{
		//	ListCtrl에 출력할 노드를 추가
		m_ListNodeArray.push_back(pNode);
		
		if( pNode->GetChildCount() > 0)
		{	
			//	자식 노드가 확장되어 있다면 자식 노드로 추가
			if( pNode->GetStyle() & REAL_PROFILE_CHILD_EXPAND)
			{
				INT32 cnt = 0;
				REAL_PROFILE_NODE * pChild = pNode->GetChild( 0);
				
				while( pChild != nullptr)
				{	
					_MakeProfilingListNode( pChild);

					pChild = pNode->GetChild( ++cnt);
				}
			}		
		}
	}
}

void i3TDKProfilingViewerCtrl::_DeleteAllProfilingList( void)
{
	m_ProfilingListCtrl.ClearAll();
}

void i3TDKProfilingViewerCtrl::_MakeProfilingHistoryGraph( void)
{
	INT32 cnt = 0;

	_DeleteAllProfilingHistoryGraph();

	I3ASSERT( m_nScrollPixel != 0);
	INT32 nMaxCount = m_nDrawWidth / (INT32) m_nScrollPixel;
	//I3TRACE( "MaxCount : %d\n", nMaxCount);

	for(size_t i = 0; i < m_ListNodeArray.size(); i++)
	{
		i3::vector<HistoryGraphNode*> GraphList;

		for( cnt = 0; cnt < nMaxCount; cnt++)
		{
			HistoryGraphNode * pGraphNode = ( HistoryGraphNode *) i3MemAlloc( sizeof( HistoryGraphNode));
			memset( pGraphNode, 0, sizeof( HistoryGraphNode));
			
			GraphList.push_back( pGraphNode);
		}

		m_HistoryGraphArray.push_back( i3::vector<HistoryGraphNode*>() );
		i3::swap( m_HistoryGraphArray.back(), GraphList);
	}
}

void i3TDKProfilingViewerCtrl::_DeleteAllProfilingHistoryGraph( void)
{
	for( size_t i = 0; i < m_HistoryGraphArray.size(); i++)
	{
		i3::vector<HistoryGraphNode*>& GraphList = m_HistoryGraphArray[i];

		for( size_t j = 0; j < GraphList.size(); j++)
		{
			HistoryGraphNode * pGraphNode = GraphList[j];

			I3MEM_SAFE_FREE( pGraphNode);
		}
	
	}

	m_HistoryGraphArray.clear();
}

void i3TDKProfilingViewerCtrl::UpdateReport( REAL_PROFILE_NODE * pRoot)
{
	INT32 count = REAL_PROFILE_GET_NODE_COUNT();

	//	프로파일 노드가 만들어 있고 Ctrl이 준비되면 Tree를 만든다.
	if( m_pRoot == nullptr && pRoot != nullptr && m_bCreate)		
	{
 		_MakeProfilingTree( pRoot);
		_MakeProfilingList( pRoot);
		_MakeProfilingHistoryGraph();
	}
	//	노드의 갯수가 변경된 경우 트리와 리스트를 새로 구성한다.
	else if( (m_TotalNodeCount != count) && m_bCreate)
	{
		_MakeProfilingTree( m_pRoot);
		_MakeProfilingList( m_pRoot);
		_MakeProfilingHistoryGraph();

		if( m_TotalNodeCount != count)	m_TotalNodeCount = count;
	}
	else
	{
		//	타이머가 활성화 되지 않으면 매프레임마다 업데이트한다.
		if( m_nTimerElapse == 0)
		{
			_UpdateTimer();
		}
	}
}

INT32 i3TDKProfilingViewerCtrl::_UpdateUsageList( REAL_PROFILE_NODE * pNode)
{
	INT32 count = 0;

	if( pNode != nullptr)
	{
		TCHAR szlabel[256];	
		REAL_PROFILE_NODE *			pCurNode = nullptr;
		REAL_PROFILE_HISTORY *		pHistory = nullptr;

		//	ListArray에 저장된 노드를 화면에 갱신한다.
		for( size_t i = 0; i < m_ListNodeArray.size(); i++)
		{
			pCurNode = m_ListNodeArray[i];
			pHistory = pCurNode->GetHistory();
			I3ASSERT( pHistory != nullptr);

			//	Update Usage
			if( pCurNode->GetParent() == nullptr)
			{
				i3::snprintf( szlabel, _countof(szlabel), _T("-- ") );
			}
			else
			{
				i3::snprintf( szlabel, _countof(szlabel), _T("%.2f "), pHistory->m_fUsageRate);
			}

			SubItemUpdateInfo updateInfo;
			updateInfo._nRowIdx = i;
			updateInfo._nColumnIdx = 1;
			updateInfo._pszText = szlabel;

			m_ProfilingListCtrl.UpdateSubItem( &updateInfo);

			//	Update Total Usage
			i3::snprintf( szlabel, _countof(szlabel), _T("%.2f "), pHistory->m_fTotalUsageRate);

			updateInfo._nRowIdx = i;
			updateInfo._nColumnIdx = 2;
			updateInfo._pszText = szlabel;

			m_ProfilingListCtrl.UpdateSubItem( &updateInfo);

			//	Update Profile Time
			i3::snprintf( szlabel, _countof(szlabel), _T("%.4f "), pHistory->m_fProfileTime);

			updateInfo._nRowIdx = i;
			updateInfo._nColumnIdx = 3;
			updateInfo._pszText = szlabel;

			m_ProfilingListCtrl.UpdateSubItem( &updateInfo);

			//	Update Calls
			i3::snprintf( szlabel, _countof(szlabel), _T("%d "), pHistory->m_TotalCalls);

			updateInfo._nRowIdx = i;
			updateInfo._nColumnIdx = 4;
			updateInfo._pszText = szlabel;

			m_ProfilingListCtrl.UpdateSubItem( &updateInfo);
		}
	}

	return count;
}

#define		COLOR_MAX	16

static COLORREF color_array[ COLOR_MAX] = 
{ 
	RGB( 255, 255, 255),	//	횐색
	RGB( 255, 50, 50),		//	빨강
	RGB( 250,150,50),		//	주황
	RGB( 255,255,50),		//	노랑
	RGB( 50,255,50),		//	초록
	RGB( 50,50,200),		//	파랑
	RGB( 150,150,255),		//	남색
	RGB( 150,50,250),		//	보라
	RGB( 224,195,105),		//	지황색	
	RGB( 122,173,76),		//	유청색
	RGB( 55,163,134),		//	진초록색
	RGB( 240,129,98),		//	주색
	RGB( 158,69,116),		//	자주색
	RGB( 253,179,109),		//	횡황색
	RGB( 132,100,159),		//	연람색
	RGB( 155,220,222),		//	옥색	
};

void i3TDKProfilingViewerCtrl::_UpdateUsageHistory( REAL_PROFILE_NODE * pNode)
{
	
	for( size_t i = 0; i < m_HistoryGraphArray.size(); i++)
	{
		REAL_PROFILE_NODE * pCurrentProflingItem = m_ListNodeArray[i];
		__CHECKF( pCurrentProflingItem != nullptr, _T("Current profiling item is null"));

		REAL_PROFILE_HISTORY * pCurrentProfilingItemHistory = pCurrentProflingItem->GetHistory();
		__CHECKF( pCurrentProfilingItemHistory != nullptr, _T("Current profiling item history is null") );

		i3::vector<HistoryGraphNode*>& GraphList = m_HistoryGraphArray[i];
		HistoryGraphNode * pUpdateNode = nullptr;

		//	맨 앞의 노드를 꺼내 뒤에 넣는다. 이렇게 하면 항상 맨 뒤에 노드는 최신 정보로 업데이트 하게 된다.
		{
			__CHECKF( GraphList.size() > 0, _T("Graph list is empty") );
			pUpdateNode = GraphList[0];
//			__CHECKF( pUpdateNode != nullptr, "Graph node is null");
			
			//	맨앞에 있는 노드를 재사용하기 위해 맨 뒤로 이동시킨다. 
			GraphList.erase(GraphList.begin());
			GraphList.push_back(pUpdateNode);
//			pGraphList->Delete( 0);				
//			pGraphList->Add( (void *) pUpdateNode);
		}

		//	그래프 정보 업데이트
		pUpdateNode->_pszName = pCurrentProfilingItemHistory->m_pszName;
		pUpdateNode->_nUsage = (INT32) ( pCurrentProfilingItemHistory->m_fTotalUsageRate + 0.5f);	//	반올림
		pUpdateNode->_color = color_array[ i % COLOR_MAX];
	}
	
	//	Update render history display
	//DrawHistory( m_ProfilingHistory.GetSafeHwnd());
}

void i3TDKProfilingViewerCtrl::_SelectProfilingNode( HTREEITEM hItem)
{
	__CHECKW( hItem != nullptr, _T("Select node is null") );

	m_pCurNode = (REAL_PROFILE_NODE *) m_ProfilingTreeCtrl.GetItemData( hItem);

	_MakeProfilingList( m_pCurNode);
	_MakeProfilingHistoryGraph();
}

void i3TDKProfilingViewerCtrl::_DrawHistoryBG( void)
{
	RECT rt;
	HPEN PenGrid, OldPenGrid;
	HBRUSH bgBrush;

	bgBrush = CreateSolidBrush( RGB( 0, 0, 0));	//	BG color

	//	BG Clear
	rt.left = 0;
	rt.top = 0;
	rt.right = m_nDrawWidth;
	rt.bottom = m_nDrawHeight;	
	::FillRect( m_hDrawDC, &rt, bgBrush);

	{							
		PenGrid = CreatePen( PS_SOLID, 1, RGB( 10, 100, 10));	//	Grid color
		OldPenGrid = (HPEN) SelectObject( m_hDrawDC, PenGrid);

		//INT32 v, h;
		//INT32 nVerticalGridGap, nHorizontalGridGap;
		//nVerticalGridGap = m_nDrawWidth / m_nWidthGridCount;
		//nHorizontalGridGap = m_nDrawHeight / m_nHeightGridCount;

		//	Draw horizontal grid lines
		INT32 nHorz = 0;		
		while( nHorz < m_nDrawHeight)
		{
			MoveToEx( m_hDrawDC, 0, nHorz, 0);
			LineTo( m_hDrawDC, m_nDrawWidth, nHorz);

			nHorz += HISTORY_GRID_PIXEL_GAP;
		}
		
		/*for( h = 0; h < m_nHeightGridCount; h++)
		{	//	Drawing horizontal grid
			MoveToEx( m_hDrawDC, 0, h * nHorizontalGridGap, 0);
			LineTo( m_hDrawDC, m_nDrawWidth, h * nHorizontalGridGap);
		}*/

		//	Draw vertical grid lines
		INT32 nVert = 0, x = 0;		
		while( nVert < m_nDrawWidth + (HISTORY_GRID_PIXEL_GAP * 2))
		{
			x = nVert - (m_TimerSec * m_nScrollPixel);

			if( x > 0)
			{
				MoveToEx( m_hDrawDC, x, 0, 0);
				LineTo( m_hDrawDC, x, m_nDrawHeight);	
			}

			nVert += HISTORY_GRID_PIXEL_GAP;
		}

		/*for( v = 0; v < m_nWidthGridCount + 2; v++)
		{	//	Drawing vertical grid
			INT32 x = (v * nVerticalGridGap) - (m_TimerSec * m_nScrollPixel);
		
			MoveToEx( m_hDrawDC, x, 0, 0);
			LineTo( m_hDrawDC, x, m_nDrawHeight);			
		}*/				

		SelectObject( m_hDrawDC, OldPenGrid);
		DeleteObject( PenGrid);
	}
}

void i3TDKProfilingViewerCtrl::_DrawHistoryGraph( void)
{
	CPaintDC dc(this); // device context for painting

	INT32 cnt = 0;
	INT32 nCurPosX = -1;
	INT32 nCurPosY = -1;
	INT32 nNextPosX = 0;
	INT32 nNextPosY = 0;
	HPEN	hPen, hOldPen;
	LOGFONT lfont;

	TCHAR tmpText[256];
	//static char * pszLongText = nullptr;
	
	HFONT oldFont = (HFONT) SelectObject( m_hDrawDC, m_CaptionFont.m_hObject);
	
	m_CaptionFont.GetLogFont( &lfont);

	//	Drawing all graph node line
	for(size_t i = 0; i < m_HistoryGraphArray.size(); i++)
	{
		i3::vector<HistoryGraphNode*>& ItemList = m_HistoryGraphArray[i];
		
		HistoryGraphNode * pNode = ItemList.back();
			//(HistoryGraphNode *) pItemList->GetLast();

		nCurPosX = -1;	//	반드시 초기화
		nCurPosY = -1;

		INT32 nY = 2;
		INT32 nX = 2;
		
		//	Graph line color
		hPen = CreatePen( PS_SOLID, 1, pNode->_color);
		hOldPen = (HPEN) SelectObject( m_hDrawDC, hPen);

		//	Drawing linked graph line
		for( cnt = 0; cnt < (INT32)ItemList.size() - 1; cnt++)
		{
			HistoryGraphNode * pCurGraphNode = ItemList[cnt];
			HistoryGraphNode * pNextGraphNode = ItemList[cnt + 1];

			if( pCurGraphNode->_pszName == nullptr)	continue;	//	비어있는 노드이므로 스킵

			if( nCurPosX == -1 && nCurPosY == -1)
			{
				nCurPosX = m_nDrawWidth - (((INT32)ItemList.size() - 1 - cnt) * m_nScrollPixel);
				//nCurPosX = cnt * m_nScrollPixel;
				nCurPosY = m_nDrawHeight - (INT32) ((REAL32) m_nDrawHeight * ((REAL32) pCurGraphNode->_nUsage * 0.01f));
			}
			
			nNextPosX = nCurPosX + m_nScrollPixel;
			nNextPosY = m_nDrawHeight - (INT32) ((REAL32) m_nDrawHeight * ((REAL32) pNextGraphNode->_nUsage * 0.01f));

			//	Draw graph line
			MoveToEx( m_hDrawDC, nCurPosX, nCurPosY, 0);
			LineTo( m_hDrawDC, nNextPosX, nNextPosY);	

			nCurPosX = nNextPosX;
			nCurPosY = nNextPosY;

			//	가장 긴 TEXT를 저장한다.
			/*if( pszLongText == nullptr)	
			{
				pszLongText = (char *) pCurGraphNode->_pszName;
			}
			else if( strlen( pszLongText) < strlen( pCurGraphNode->_pszName))
			{
				pszLongText = (char *) pCurGraphNode->_pszName;
			}*/
		}

		SelectObject( m_hDrawDC, hOldPen);
		DeleteObject( hPen);

		//	Text
		{
			//HDC hdc = ::GetDC( m_ProfilingHistory.GetSafeHwnd());
			//SIZE	size;
			//INT32 cnt = 1;

			nY = i * lfont.lfHeight;

			if( nY + lfont.lfHeight > m_nDrawHeight)		
				nY = m_nDrawHeight - lfont.lfHeight;
			
			//	스크린 크기가 일정 이상 작아지면 옆으로 출력되게 
			/*while( nY + textMetric.tmHeight > m_nDrawHeight)
			{
				nY = (nY + textMetric.tmHeight) % m_nDrawHeight;
				cnt = (nY + textMetric.tmHeight) / m_nDrawHeight;

				GetTextExtentPoint32( hdc, pszLongText, strlen( pszLongText), &size);
				nX += size.cx * cnt;

				cnt++;
			}*/

			::SetBkMode( m_hDrawDC, TRANSPARENT);
			::SetTextColor( m_hDrawDC, pNode->_color);		
			i3::snprintf( tmpText, _countof(tmpText), _T("%s"), pNode->_pszName);
			TextOut( m_hDrawDC, nX, nY, tmpText, (INT32) i3::generic_string_size( tmpText) );		
		}
	}

	//	Draw FPS

#if !defined( I3_COMPILER_VC)
	INT32 nFps = REAL_PROFILE_GET_FPS();
	if( nFps >= 0)
	{		
		::SetTextColor( m_hDrawDC, RGB( 220, 220, 220));
		i3::snprintf( tmpText, _countof(tmpText), _T("fps %d"), nFps);
		TextOut( m_hDrawDC, m_nDrawWidth - 50, 2, tmpText, (INT32) i3::generic_string_size( tmpText));
	}
#endif
	SelectObject( m_hDrawDC, oldFont);
}

void i3TDKProfilingViewerCtrl::_UpdateTimer( void)
{
	//	Update time	
	_CalcTimeSec();

	_UpdateUsageHistory( m_pCurNode);
	_UpdateUsageList( m_pCurNode);

	InvalidateRect( nullptr, FALSE);
}

void i3TDKProfilingViewerCtrl::_CalcTimeSec( void)
{
	//m_TimerSec = ++m_TimerSec % (m_nDrawWidth / m_nWidthGridCount) + m_nScrollPixel);
	INT32 timeGap = (m_nDrawWidth / m_nWidthGridCount) / m_nScrollPixel;

	if( m_TimerSec < timeGap)
		m_TimerSec++;
	else
		m_TimerSec = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
int i3TDKProfilingViewerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitMain();

	return 0;
}

void i3TDKProfilingViewerCtrl::ProfilingTreeCtrl_SetSize( CRect * pRect)
{
	m_rtProfilingTreeCtrl.CopyRect( pRect);

	m_ProfilingTreeCtrl.SetWindowPos( nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void i3TDKProfilingViewerCtrl::ProfilingListCtrl_SetSize( CRect * pRect)
{
	m_rtProfilingListCtrl.CopyRect( pRect);

	m_ProfilingListCtrl.SetWindowPos( nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void i3TDKProfilingViewerCtrl::ProfilingHistory_SetSize( CRect * pRect)
{
	m_rtProfilingHistory.CopyRect( pRect);

	m_ProfilingHistory.SetWindowPos( nullptr, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	m_nDrawWidth = pRect->Width();			//	History View width
	m_nDrawHeight = pRect->Height();		//	History View height

	//	Scroll Pixel Width
	m_nScrollPixel = (m_nDrawWidth / m_nWidthGridCount) / 2;
	if( m_nScrollPixel < 1)	m_nScrollPixel = 1;

	//I3TRACE("PIXEL %d (%f)\n", m_nScrollPixel, pixel);
}

void i3TDKProfilingViewerCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your specialized code here and/or call the base class

	//	Resizing TreeCtrl
	if( ::IsWindow( m_ProfilingTreeCtrl.GetSafeHwnd()))
	{
		CRect rect;

		GetClientRect( &rect);

		rect.left += VIEWER_CTRL_PIXEL_GAP;
		rect.top += VIEWER_CTRL_PIXEL_GAP;
		rect.right = rect.Width() / 3;
		rect.bottom -= VIEWER_CTRL_PIXEL_GAP;
		
		ProfilingTreeCtrl_SetSize( &rect);
	}

	//	Resizing ListCtrl
	if( ::IsWindow( m_ProfilingListCtrl.GetSafeHwnd()))
	{
		CRect rect;

		GetClientRect( &rect);

		rect.left = (rect.Width() / 3) + VIEWER_CTRL_PIXEL_GAP;
		rect.top += VIEWER_CTRL_PIXEL_GAP;
		rect.right -= VIEWER_CTRL_PIXEL_GAP;
		rect.bottom = (INT32) ((REAL32) rect.Height() * 0.5f);
		
		ProfilingListCtrl_SetSize( &rect);
	}

	//	Resizing usage history view
	if( ::IsWindow( m_ProfilingHistory.GetSafeHwnd()))
	{
		CRect rect;

		GetClientRect( &rect);

		rect.left = (rect.Width() / 3) + VIEWER_CTRL_PIXEL_GAP;
		rect.top = (INT32) ((REAL32) rect.Height() * 0.5f) + VIEWER_CTRL_PIXEL_GAP;
		rect.right -= VIEWER_CTRL_PIXEL_GAP;
		rect.bottom -= VIEWER_CTRL_PIXEL_GAP + 20;
		
		ProfilingHistory_SetSize( &rect);

		//	Recreate History View
		{
			::SelectObject( m_hDrawDC, m_hbmpOld);
			::DeleteObject( m_hbmpDraw);

			::SetMapMode( m_hDrawDC, MM_TEXT);
			::SetWindowExtEx( m_hDrawDC, m_nDrawWidth, m_nDrawHeight, 0);
			::SetViewportExtEx( m_hDrawDC, m_nDrawWidth, m_nDrawHeight, 0);

			if( m_nDrawWidth <= 0)	m_nDrawWidth = 1;
			if( m_nDrawHeight <= 0)	m_nDrawHeight = 1;

			//	Create drawing bitmap
			m_hbmpDraw = CreateBitmap( m_nDrawWidth, m_nDrawHeight, 
										GetDeviceCaps( m_hDrawDC, PLANES),
										GetDeviceCaps( m_hDrawDC, BITSPIXEL), nullptr);
			I3ASSERT( m_hbmpDraw != nullptr);

			m_hbmpOld = (HBITMAP) ::SelectObject( m_hDrawDC, m_hbmpDraw);
		}

		::InvalidateRect( m_ProfilingHistory.GetSafeHwnd(), rect, FALSE);
	}	

	{
		RECT rect;

		//	UpdateTime Edit
		GetClientRect( &rect);

		rect.left = rect.right - 110;
		rect.top = rect.bottom - 35;
		rect.right -= 70;
		rect.bottom -= 15;
		
		m_UpdateTimeEdit.SetWindowPos( nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER);

		//	Apply Button
		GetClientRect( &rect);

		rect.left = rect.right - 65;
		rect.top = rect.bottom - 35;
		rect.right -= 20;
		rect.bottom -= 15;

		m_BtnApply.SetWindowPos( nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BOOL i3TDKProfilingViewerCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify(wParam, lParam, pResult);
}

LRESULT i3TDKProfilingViewerCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message)
	{	
	case WM_COMMAND:
		{
			switch( LOWORD( wParam))
			{
			case ID_BTN_APPLY:			//	Apply Button
				{
					TCHAR szVal[256];
					INT32 nVal = 0;

					m_UpdateTimeEdit.GetWindowText( szVal, _countof(szVal) );

					nVal = i3::atoi( szVal);

					if( nVal != 0)
					{	//	UpdateTime 변경
						SetUpdateTime( nVal );
					}
					else
					{
						i3::snprintf( szVal, _countof(szVal), _T("%d"), m_nTimerElapse);
						m_UpdateTimeEdit.SetWindowText( szVal);
					}
				}
				break;
			case ID_UPDATE_TIME_EDIT:	//	Update Time Edit-box
				{					
				}
				break;
			}
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR			hdr = (LPNMHDR) lParam;
			LPNMTREEVIEW	ntv = (LPNMTREEVIEW) lParam;

			switch( hdr->code)
			{	
				case TVN_ITEMEXPANDED:
					{
						TVITEM * selectItemInfo = &ntv->itemNew;						
						HTREEITEM selectItem = selectItemInfo->hItem;

						REAL_PROFILE_NODE * pSelectNode = (REAL_PROFILE_NODE *) m_ProfilingTreeCtrl.GetItemData( selectItem);

						//	아이템이 펼쳐질때
						if( selectItemInfo->state & TVIS_EXPANDED)
						{						
							pSelectNode->AddStyle( REAL_PROFILE_CHILD_EXPAND);

							_MakeProfilingList( m_pCurNode);
							_MakeProfilingHistoryGraph();
						}
						//	아이템이 접힐때
						else if( selectItemInfo->state & TVIS_EXPANDEDONCE)
						{
							pSelectNode->RemoveStyle( REAL_PROFILE_CHILD_EXPAND);

							_MakeProfilingList( m_pCurNode);
							_MakeProfilingHistoryGraph();
						}
						
					}
					break;
				case TVN_SELCHANGING:
					{

					}
					break;
			}
		}
		break;
	
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void i3TDKProfilingViewerCtrl::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//HWND hWnd = m_ProfilingHistory.GetSafeHwnd();

	//	Ctrl Update & Drawing
	if( nIDEvent == ID_TIMER_USAGE_HISTORY_REDRAW)
	{	
		_UpdateTimer();
	}

	CWnd::OnTimer(nIDEvent);
}

void i3TDKProfilingViewerCtrl::_DrawGroupBox( const TCHAR * pszCaption, INT32 x1, INT32 y1, INT32 x2, INT32 y2)
{
	RECT rect = { x1, y1, x2, y2};

	_DrawGroupBox( pszCaption, &rect);
}

void i3TDKProfilingViewerCtrl::_DrawGroupBox( const TCHAR * pszCaption, RECT * rt)
{
	CDC * pDc = GetDC();	

	if( pDc == nullptr)	return;

	CPoint ptStart, ptEnd; 
	CPen pnFrmDark, pnFrmLight, *ppnOldPen;
	CRect rc;

	rc.SetRect( rt->left + 5, rt->top + 5, rt->right - 5, rt->bottom - 5);

	VERIFY(pnFrmDark.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW)));
	VERIFY(pnFrmLight.CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHILIGHT)));

	ppnOldPen = pDc->SelectObject(&pnFrmDark);

	//	Draw Group-box line
	pDc->Rectangle( &rc);

	/*pDc->SelectObject(&pnFrmLight);

	rc.left += 1;
	rc.top += 1;
	rc.right += 1;
	rc.bottom += 1;

	pDc->Rectangle( &rc);*/

	pDc->SelectObject(ppnOldPen);

	//	Caption in Group-box
	{	
		CFont * pOldFont = pDc->SelectObject( &m_CaptionFont);

		pDc->TextOut( rt->left + 25, rt->top, pszCaption);

		pDc->SelectObject( pOldFont);
	}	
}

void i3TDKProfilingViewerCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	HDC hdc;	
	PAINTSTRUCT ps;

	//	Draw Group-box
	{
		RECT rt;
		rt.left = m_rtProfilingTreeCtrl.left - 15;
		rt.top = m_rtProfilingTreeCtrl.top - 15;
		rt.right = m_rtProfilingTreeCtrl.right + 15;
		rt.bottom = m_rtProfilingTreeCtrl.bottom + 15;

		_DrawGroupBox( _T("Profile"), &rt);	

		rt.left = m_rtProfilingListCtrl.left - 15;
		rt.top = m_rtProfilingListCtrl.top - 15;
		rt.right = m_rtProfilingListCtrl.right + 15;
		rt.bottom = m_rtProfilingListCtrl.bottom + 15;

		_DrawGroupBox( _T("System Usage"), &rt);	

		rt.left = m_rtProfilingHistory.left - 15;
		rt.top = m_rtProfilingHistory.top - 15;
		rt.right = m_rtProfilingHistory.right + 15;
		//rt.bottom = m_rtProfilingHistory.bottom + 15;
		rt.bottom = m_rtProfilingTreeCtrl.bottom + 15;

		_DrawGroupBox( _T("History"), &rt);	
			
		//SetTextColor( pDc->GetSafeHdc(), RGB( 0, 0, 0));
		RECT rect;
		GetClientRect( &rect);
		CFont * pOldFont = dc.SelectObject( &m_CaptionFont);

		dc.TextOut( rect.right - 188, rect.bottom - 32, _T("UpdateTime(ms)") );

		dc.SelectObject( pOldFont);
		
	}
		
	HWND hWnd = m_ProfilingHistory.GetSafeHwnd();
	//	Draw begin 	
	hdc = ::BeginPaint( hWnd, &ps);
	{
		_DrawHistoryBG();
		_DrawHistoryGraph();
	}
	//	Draw end
	::EndPaint( hWnd, &ps);	

	//	BitBlt to View
	::StretchBlt( hdc, 0, 0, m_nDrawWidth, m_nDrawHeight, m_hDrawDC, 0, 0, m_nDrawWidth, m_nDrawHeight, SRCCOPY);
}

void i3TDKProfilingViewerCtrl::OnNMLClickTree( NMHDR * pNMHDR, LRESULT * pResult)
{
	POINT mouse_pt, pt;
	HTREEITEM hHitItem = nullptr;
	UINT flag = 0;

	//	Mouse Click Node in CtrlTree
	GetCursorPos( &mouse_pt);
	pt = mouse_pt;
	m_ProfilingTreeCtrl.ScreenToClient( &pt);

	hHitItem = m_ProfilingTreeCtrl.HitTest( pt, &flag);

	if( (hHitItem != nullptr) && (flag & TVHT_ONITEM))
	{
		_SelectProfilingNode( hHitItem);
	}
	//
	*pResult = 0;
}

#endif

