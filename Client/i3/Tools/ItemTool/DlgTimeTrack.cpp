// DlgTimeTrack.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgTimeTrack.h"


// cDlgTimeTrack 대화 상자입니다.

IMPLEMENT_DYNAMIC(cDlgTimeTrack, CDialog)

cDlgTimeTrack::cDlgTimeTrack(CWnd* pParent /*=NULL*/)
: CDialog(cDlgTimeTrack::IDD, pParent)
,	m_HwndParent(NULL)
,	m_pAIState(NULL)
,	m_nSelectedIndex(0)
{
}

cDlgTimeTrack::~cDlgTimeTrack()
{
}

void cDlgTimeTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void cDlgTimeTrack::SetAIState(GICShapeAI* pShape)
{
	m_pAIState = pShape;

	m_TimeSeqCtrl.SetShape( pShape);
}

BEGIN_MESSAGE_MAP(cDlgTimeTrack, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL cDlgTimeTrack::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_TimeSeqCtrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 100);
	}

	g_HwndDlgTimeTrack = GetSafeHwnd();
	//i3TDK::RegisterUpdate( m_hWnd, GICShapeAI::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;
}

void cDlgTimeTrack::OnMove(HWND hWndParent)
{
	if (hWndParent == NULL)
		return;

	m_HwndParent = hWndParent;

	//RECT clientRc;
	RECT rc0, rc1;
		
	::GetWindowRect( hWndParent, &rc0 );
	::GetWindowRect( GetSafeHwnd(), &rc1 );

 	int height = rc1.bottom - rc1.top;
 	int y = rc0.bottom;// - height;
 	if( y > g_pViewer->GetMaxH() - height)
 		y = g_pViewer->GetMaxH() - height;
 	int x = rc0.left;


	::SetWindowPos( GetSafeHwnd(), 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}

void cDlgTimeTrack::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);
	if( ::IsWindow( m_TimeSeqCtrl.m_hWnd))
	{
		m_TimeSeqCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


LRESULT cDlgTimeTrack::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		I3_TDK_UPDATE_INFO* pInfo = (I3_TDK_UPDATE_INFO*)wParam;
		if (pInfo->m_Event == I3_TDK_UPDATE_SELECT)
		{
			//if (m_TimeSeqCtrl.GetMouseState() != 5)
			{
				float fCurTime = ((float)lParam) / 10000.0f;

				m_TimeSeqCtrl.EnsureVisible(fCurTime);
				m_TimeSeqCtrl.SetTimeGauage(fCurTime);
			}

			m_TimeSeqCtrl.Invalidate();
			m_TimeSeqCtrl._Draw();
		}
		else if(pInfo->m_Event == I3_TDK_UPDATE_ADD)
		{
			UpdateAll(m_nSelectedIndex);

			m_TimeSeqCtrl.EnsureVisible(0);
			m_TimeSeqCtrl.SetTimeGauage(0);

			m_TimeSeqCtrl.Invalidate();
			m_TimeSeqCtrl._Draw();
// 			RECT rc;
// 			GetClientRect(&rc);
// 			m_TimeSeqCtrl.SetWindowPos( NULL, 0, 0, rc.left, rc.top, SWP_NOACTIVATE | SWP_NOZORDER);
			//m_TimeSeqCtrl.Invalidate();
			//m_TimeSeqCtrl.ReDraw();
			
		}
		else
			UpdateAll(lParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void cDlgTimeTrack::UpdateAll(int nIdx)
{
	m_nSelectedIndex = nIdx;
	
	if (g_pChara->getAICount() == 0)
		return;

	AI* pAI						= g_pChara->getAI(0);
	i3AI* pCurrAI				= pAI->getAI();
	i3AIState* pCurrAiState		= pCurrAI->getAIState(nIdx);
	//i3Animation* pAnim			= pCurrAiState->getAnim();
	CString m_strFileName		= pCurrAiState->GetName();

	GICShapeAI* pShape = (GICShapeAI*)pAI->getDiagram()->FindShapeByName(m_strFileName);

	I3ASSERT(pShape != NULL);
	SetAIState(pShape);
}