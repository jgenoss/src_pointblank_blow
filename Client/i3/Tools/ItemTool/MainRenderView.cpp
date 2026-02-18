#include "stdafx.h"
#include "resource.h"
//#include "ItemToolView.h"
#include "MainRenderview.h"
#include "DlgWeaponEditor.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cMainRenderView::cMainRenderView()
{
	g_pWeaponSCFramework = NULL;
	g_pItemSCFramework = NULL;
}

cMainRenderView::~cMainRenderView()
{
	I3_SAFE_RELEASE(g_pChara);

	I3_SAFE_RELEASE(g_pMainSCFramework);
	I3_SAFE_RELEASE(g_pWeaponSCFramework);
	I3_SAFE_RELEASE(g_pItemSCFramework);
	I3_SAFE_RELEASE(g_pViewer);
}


BEGIN_MESSAGE_MAP(cMainRenderView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()

	ON_WM_VSCROLL()
END_MESSAGE_MAP()



BOOL cMainRenderView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

int cMainRenderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			g_pViewer = (cItemToolViewer*)m_ViewerCtrl.getViewer();

			g_pMainSCFramework = cItemToolMainFrameWork::new_object();
			g_pMainSCFramework->Create( m_ViewerCtrl.getViewer());
			g_pMainSCFramework->SetType(E_SWAPCHAIN_MAIN);
			g_pMainSCFramework->SetName("MainSCFrm");
			g_pViewer->SetSwapChainSgContext( g_pMainSCFramework->getSgContext(), E_SWAPCHAIN_MAIN);
			g_pViewer->SetWeaponHwnd(m_ViewerCtrl.GetSafeHwnd(),E_SWAPCHAIN_MAIN);
			g_pViewer->SetSCFrameWork(g_pMainSCFramework,E_SWAPCHAIN_MAIN);

			g_pWeaponSCFramework = cItemToolSubFrameWork::new_object();
			g_pWeaponSCFramework->Create( m_ViewerCtrl.getViewer());
			g_pWeaponSCFramework->SetType(E_SWAPCHAIN_WEAPON);
			g_pWeaponSCFramework->SetName("WeaponSCFrm");
			g_pViewer->SetSwapChainSgContext( g_pWeaponSCFramework->getSgContext(), E_SWAPCHAIN_WEAPON);
			g_pViewer->SetSCFrameWork(g_pWeaponSCFramework,E_SWAPCHAIN_WEAPON);

			g_pItemSCFramework = cItemToolSubFrameWork::new_object();
			g_pItemSCFramework->Create( m_ViewerCtrl.getViewer());
			g_pItemSCFramework->SetType(E_SWAPCHAIN_ITEM);
			g_pItemSCFramework->SetName("ItemSCFrm");
			g_pViewer->SetSwapChainSgContext( g_pItemSCFramework->getSgContext(), E_SWAPCHAIN_ITEM);
			g_pViewer->SetSCFrameWork(g_pItemSCFramework,E_SWAPCHAIN_ITEM);

			((cItemToolViewer*)g_pViewer)->CreateLater();

		}

		i3RenderContext * pCtx = m_ViewerCtrl.getViewer()->GetRenderContext();
		
		COLOR col;
		i3Color::Set( &col, 0.2f,0.2f,0.2f,1.0f);
		pCtx->SetClearColor(&col);

		rect.SetRect( 0, 0, 30, 100);
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_ALL);
	}


	g_pChara	= CChara::new_object();

	return 0;
}


const int	Ctrl_Interval	= 3;
const int	ScrollBar_Width	= 20;

void cMainRenderView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(::IsWindow(m_ViewerCtrl.m_hWnd))
	{
		int		nPosX	= Ctrl_Interval;
		int		nPosY	= Ctrl_Interval;
		int		nSizeX	= cx - Ctrl_Interval * 2;
		int		nSizeY	= cy - Ctrl_Interval * 2;

		m_ViewerCtrl.SetWindowPos( NULL, nPosX, nPosY, nSizeX, nSizeY, SWP_NOACTIVATE | SWP_NOZORDER );
	}

	RECT rtRender;
	if (g_pViewer && GetSafeHwnd())
	{
		GetClientRect(&rtRender);
//		g_pViewer->SetWeaponRect(rtRender, E_SWAPCHAIN_MAIN);
	}
}

void cMainRenderView::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	m_ViewerCtrl.SetFocus();
}


void cMainRenderView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( pScrollBar == NULL )
	{
		switch( nSBCode )
		{
		case	SB_LINEUP :
			AddScrollBarPos(-1);
			break;
		case	SB_LINEDOWN :
			AddScrollBarPos(1);
			break;
		case	SB_PAGEUP :
			AddScrollBarPos(-g_pMainSCFramework->GetColPage());
			break;
		case	SB_PAGEDOWN :
			AddScrollBarPos(g_pMainSCFramework->GetColPage());
			break;

		case	SB_THUMBPOSITION	:
			_MoveScrollBarPos(nPos);
			break;
		}
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}



void cMainRenderView::ResetItemList( void )
{
	if( g_pMainSCFramework == NULL )
		return;

 	SetScrollRange(SB_VERT, 0, g_pMainSCFramework->GetScrollCount());
	_MoveScrollBarPos(0);

	g_pMainSCFramework->ResizeUI();
}

void cMainRenderView::_MoveScrollBarPos(INT32 nPos)
{
	INT32		nMin	= 0;
	INT32		nMax	= 0;


	GetScrollRange(SB_VERT, &nMin, &nMax);

	if( nMin > nPos )
		nPos	= nMin;
	if( nMax < nPos )
		nPos	= nMax;

	g_pMainSCFramework->ResetItemList(nPos);
	SetScrollPos(SB_VERT, nPos);
}

void cMainRenderView::AddScrollBarPos(INT32 nValue)
{
	INT32		nPos	= GetScrollPos(SB_VERT) + nValue;

	_MoveScrollBarPos(nPos);
}
